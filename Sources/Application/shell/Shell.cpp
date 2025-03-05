#include <OsintgramCXX/App/Shell/Shell.hpp>

#include <OsintgramCXX/Commons/Utils.hpp>
#include <OsintgramCXX/Commons/Terminal.hpp>
#include <OsintgramCXX/Commons/Tools.hpp>
#include <OsintgramCXX/Commons/HelpPage.hpp>

#include <sstream>
#include <thread>
#include <iostream>
#include <filesystem>
#include <cstring>

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

struct CommandExecution {
    bool cmdFound;
    int rc;
};

void helpCmd() {
    if (OsintgramCXX::ShellFuckery::shellCommands.empty()) {
        std::cerr << "No commands have been added." << std::endl;
        std::cerr << "To add commands, load a native library with the use of 'commands.json' file," << std::endl;
        std::cerr << "and restart the application." << std::endl;
        std::cerr << "Safely exit out of the application by typing 'exit', 'quit' or 'close'" << std::endl;
        return;
    }

    HelpPage page;
    page.setSpaceWidth(5);
    page.setStartSpaceWidth(2);

    for (const auto &cmdVar: OsintgramCXX::ShellFuckery::shellCommands)
        page.addArg(cmdVar.commandName, "", cmdVar.quickHelpStr);

    page.addArg("exit", "", "Exits the application");
    page.display(std::cout);
}

void forceStopShell(int) {
    OsintgramCXX::ShellFuckery::stopShell(true);
    OsintgramCXX::ShellFuckery::cleanup();
    std::cin.setstate(std::ios::badbit);
}

int execCommand(const std::string &cmd, const std::vector<std::string> &args, const ShellEnvironment &env,
                const std::string &cmdLine) {
    bool found = false;
    ModHandles::C_CommandExec cmdExecHandle = nullptr;

    for (const auto &it: ModHandles::loadedLibraries) {
        for (const auto &cmdEntry: it.second.commands) {
            if (cmd == cmdEntry.cmd) {
                found = true;
                cmdExecHandle = cmdEntry.execHandler;
                break;
            }
        }
    }

    if (cmdExecHandle == nullptr) {
        std::cerr << "Command \"" << cmd << "\" does not have a handler for execution." << std::endl;
        return 3;
    }

    if (!found) {
        std::cerr << cmd << ": command not found" << std::endl;
        return 2;
    }

    char **argv = new char *[args.size()];
    for (size_t i = 0; i < args.size(); i++) {
        argv[i] = strdup(args[i].c_str());
    }

    char **env_map = new char *[env.size()];
    int index = 0;
    for (const auto &[key, value]: env) {
        std::string entry = key;
        entry.append("=").append(value);

        env_map[index++] = strdup(entry.c_str());
    }

    // start the listeners for "OnCommandExec"
    for (const auto &it: ModHandles::loadedLibraries) {
        std::thread t([handler = it.second, &cmdLine]() {
            if (handler.handler_onCmdExec != nullptr)
                handler.handler_onCmdExec(const_cast<char *>(cmdLine.c_str()));
        });
        t.detach();
    }

    int rc = 3;
    try {
        rc = cmdExecHandle(cmd.c_str(), args.size(), argv, env.size(), env_map);
    } catch (const std::runtime_error &err) {
        std::cerr << "Runtime error occured, while executing \"" << cmd << "\": " << err.what() << std::endl;
    } catch (const std::exception &err) {
        std::cerr << "Error occured, while executing \"" << cmd << "\": " << err.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error occured, while executing \"" << cmd << "\"" << std::endl;
    }

    for (int i = 0; i < env.size(); i++) {
        free(env_map[i]);
    }
    delete[] env_map;

    for (int i = 0; i < args.size(); i++) {
        free(argv[i]);
    }
    delete[] argv;

    return rc;
}

namespace OsintgramCXX::ShellFuckery {

    std::string PS1;
    bool running = false;
    [[maybe_unused]] bool shellInitialized = false;
    ShellEnvironment environment;
    std::vector<ShellCommand> shellCommands{};
    std::thread shellThread;
    bool alreadyForceStopped = false;

    void add_command(const ShellCommand &cmd) {
        shellCommands.push_back(cmd);
    }

    void initializeShell() {
        alreadyForceStopped = false;

        std::string user = CurrentUsername();
        std::string sCwd = CurrentWorkingDirectory();
        fs::path cwd = fs::current_path();

        std::stringstream strStream;
        strStream << "[" << user << "/" << GetHostname() << ": " << cwd.filename().string() << "]"
                  << (IsAdmin() ? "#" : "$") << " ";

        PS1 = strStream.str();

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

                if (line == "help") {
                    helpCmd();
                    continue;
                }

                std::vector<std::string> cmdLine = Tools::translateCmdLine(line);
                std::vector<std::string> cmdArgs;

                if (cmdLine.size() > 1) {
                    for (size_t i = 1; i < cmdLine.size(); i++)
                        cmdArgs.push_back(cmdLine[i]);
                }

                std::vector<std::string> _cmdArgs = cmdArgs;

                bool cmdFound = false;

                for (const auto &cmdEntry: shellCommands) {
                    if (cmdEntry.commandName == cmdLine[0]) {
                        int rc = execCommand(cmdLine[0], cmdArgs, environment, line);

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
            std::cerr << "Shell not initialized" << std::endl;
            return;
        }

        running = true;

        try {
            shellThread = std::thread(cmd);
            shellThread.join();
        } catch (std::exception &ex) {
            std::cerr << "Shell Thread Error (Shell.cpp): " << ex.what() << std::endl;
            stopShell(false);
        }
    }

    void stopShell(bool forceStop) {
        running = false;

        if (forceStop && !alreadyForceStopped) {
#if defined(__ANDROID__)
            pthread_kill(shellThread.native_handle(), SIGKILL);
#elif !defined(__ANDROID__) && defined(__linux__)
            pthread_cancel(shellThread.native_handle());
#elif defined(_WIN32)
            TerminateThread(reinterpret_cast<HANDLE>(shellThread.native_handle()), 0);
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