#include <OsintgramCXX/App/Shell/Shell.hpp>

#include <OsintgramCXX/Commons/Utils.hpp>
#include <OsintgramCXX/Commons/Terminal.hpp>
#include <OsintgramCXX/Commons/Tools.hpp>
#include <OsintgramCXX/Commons/HelpPage.hpp>

#include "cmd/ShellCommandEntries.hpp"

#include <sstream>
#include <thread>
#include <iostream>
#include <filesystem>

#ifdef __linux__
#include <csignal>
#include <pthread.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#elif _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

using namespace OsintgramCXX;

namespace fs = std::filesystem;

int helpCmd(const std::vector<std::string> &, const ShellEnvironment &) {
    HelpPage page;
    page.setSpaceWidth(5);
    page.setStartSpaceWidth(2);

    for (const auto &cmdVar: OsintgramCXX::ShellFuckery::shellCommands)
        page.addArg(cmdVar.commandName, "", cmdVar.quickHelpStr);

    page.addArg("exit", "", "Exits the application");

    page.display(std::cout);

    return 0;
}

std::string helpStrCmd() {
    // this will likely never be called
    return "A help command, what do you expect?";
}

int cmd_writeSettings(const std::vector<std::string> &ar, const ShellEnvironment &vn) {
    std::string fPath;

#ifdef __linux__
    struct passwd* pw = getpwuid(getuid());
    if (pw && pw->pw_dir) {
        fPath = std::string(pw->pw_dir);
        fPath.append("/.config/net.bc100dev/OsintgramCXX/Settings.env");
    }
#elif _WIN32
    char homeDir[MAX_PATH_LIMIT];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, homeDir))){
        fPath = std::string(homeDir);
        fPath.append(R"(\AppData\Local\bc100dev.net\OsintgramCXX\Settings.env)");
    }
#endif

    if (fPath.empty()) {
        std::cerr << "Failed to fetch home directory" << std::endl;
        return 1;
    }

    return 0;
}

std::string help_writeSettings() {
    return "To be implemented";
}

void forceStopShell(int) {
    OsintgramCXX::ShellFuckery::stopShell(true);
    OsintgramCXX::ShellFuckery::cleanup();
    std::cin.setstate(std::ios::badbit);
}

// why did MinGW now decide to make the conflict with "Shell" against my "OsintgramCXX::Shell"
// makes no fucking sense
// did someone in C++ development experience something similar? If not, then I guess it's just me, constantly battling
// well, now it is ShellFuckery
namespace OsintgramCXX::ShellFuckery {

    std::string PS1;
    bool running = false;
    [[maybe_unused]] bool shellInitialized = false;
    ShellEnvironment environment;
    std::vector<ShellCommand> shellCommands{};
    std::thread shellThread;
    bool alreadyForceStopped = false;

    void initializeShell() {
        alreadyForceStopped = false;

        std::string user = CurrentUsername();
        std::string sCwd = CurrentWorkingDirectory();
        fs::path cwd = fs::current_path();

        std::stringstream strStream;
        strStream << "[" << user << "/" << GetHostname() << ": " << cwd.filename().string() << "]"
                  << (IsAdmin() ? "#" : "$") << " ";

        PS1 = strStream.str();

        ShellCommand vHelpCmd;
        vHelpCmd.commandName = "help";
        vHelpCmd.quickHelpStr = "List all commands and features";
        vHelpCmd.executionCommand = helpCmd;
        vHelpCmd.helpCommand = helpStrCmd;

        ShellCommand vWriteSettings;
        vWriteSettings.commandName = "write-settings";
        vWriteSettings.quickHelpStr = "Write ShellFuckery environment variables to the Settings file";
        vWriteSettings.executionCommand = cmd_writeSettings;
        vWriteSettings.helpCommand = help_writeSettings;

        shellCommands.push_back(vHelpCmd);
        shellCommands.push_back(vWriteSettings);

        std::vector<ShellCommand> cmdList = AppShell::Commands::importCommands();
        for (const auto& it : cmdList)
            shellCommands.push_back(it);

        shellInitialized = true;
    }

    void modEnviron(const std::string &line) {
        std::string worker = TrimString(line);
        if (worker[0] == '&')
            worker = worker.substr(1);

        if (StringContains(line, "=")) {
            std::vector<std::string> opt = SplitString(worker, "=", 2);
            opt[0] = TrimString(opt[0]);
            opt[1] = TrimString(opt[1]);

            if (opt[0] == "PS1") {
                PS1 = opt[1];
                return;
            }

            environment[opt[0]] = opt[1];
        } else {
            auto it = environment.find(worker);
            if (it == environment.end()) {
                Terminal::errPrintln(Terminal::TermColor::RED, worker + " (not found)", true);
                return;
            }

            Terminal::print(Terminal::TermColor::BLUE, worker, true);
            std::cout << " => ";
            Terminal::print(Terminal::TermColor::CYAN, it->second, true);
        }
    }

    void cmd() {
#ifdef __linux__
        signal(SIGINT, forceStopShell);
        signal(SIGTERM, forceStopShell);
        signal(SIGKILL, forceStopShell);
        signal(SIGABRT, forceStopShell);
#endif

        std::string line;

        while (running) {
            try {
                std::cout << PS1;
                std::getline(std::cin, line);

                line = TrimString(line);

                // forgotten lines (would SegFault otherwise)
                if (line.empty())
                    continue;

                if (line[0] == '&')
                    modEnviron(line);

                if (line == "exit" || line == "quit" || line == "close") {
                    stopShell(false);
                    return;
                }

                std::vector<std::string> cmdLine = Tools::translateCmdLine(line);
                std::vector<std::string> cmdArgs;

                if (cmdLine.size() > 1) {
                    for (size_t i = 1; i < cmdLine.size(); i++)
                        cmdArgs.push_back(cmdLine[i]);
                }

                std::vector<std::string> _cmdArgs = cmdArgs;

                bool cmdFound = false;
                std::string helpFetchCmd;

                if (cmdLine[0] == "help") {
                    cmdFound = true;

                    for (int i = 0; i < _cmdArgs.size(); i++) {
                        if (cmdArgs[i] == "help")
                            cmdArgs.erase(cmdArgs.begin() + i);
                    }

                    if (!cmdArgs.empty()) {
                        helpFetchCmd = cmdArgs[0];
                        cmdArgs.erase(cmdArgs.begin());
                    }
                }

                if (cmdFound && helpFetchCmd.empty()) {
                    shellCommands[0].executionCommand(cmdArgs, environment);
                    continue;
                }

                for (const auto &cmdEntry: shellCommands) {
                    if (cmdFound && !helpFetchCmd.empty()) {
                        if (cmdEntry.commandName == helpFetchCmd) {
                            std::cout << cmdEntry.helpCommand() << std::endl;
                            break;
                        }
                    } else if (cmdEntry.commandName == cmdLine[0]) {
                        int rc = cmdEntry.executionCommand(cmdArgs, environment);
                        if (rc != 0)
                            std::cerr << cmdLine[0] << ": returned " << rc << std::endl;

                        cmdFound = true;
                        break;
                    }
                }

                if (!cmdFound) {
                    std::cerr << cmdLine[0] << ": command not found" << std::endl;
                    CurrentThread_Sleep(90);
                }
            } catch (const std::exception &ex) {
                std::cerr << "ShellError: " << ex.what() << std::endl;
            }
        }
    }

    void launchShell() {
        if (!shellInitialized) {
            std::cerr << "ShellFuckery not initialized" << std::endl;
            return;
        }

        running = true;
        std::string line;

        try {
            shellThread = std::thread(cmd);
            shellThread.join();
        } catch (std::exception &ex) {
            std::cerr << "ShellFuckery Thread Error (ShellFuckery.cpp): " << ex.what() << std::endl;
            stopShell(false);
        }
    }

    void stopShell(bool forceStop) {
        running = false;

        if (forceStop && !alreadyForceStopped) {
#ifdef __linux__
            pthread_cancel(shellThread.native_handle());
#elif _WIN32
            TerminateThread(shellThread.native_handle(), 0);
#endif

            shellThread.detach();
            alreadyForceStopped = true;

            cleanup();
        }
    }

    void cleanup() {
        PS1 = "";
        environment.clear();
        shellCommands.clear();
        shellInitialized = false;
    }

}