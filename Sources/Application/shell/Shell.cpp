#include <OsintgramCXX/App/Shell/Shell.hpp>
#include <OsintgramCXX/App/ModHandles.hpp>

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
#include <pwd.h>

#elif _WIN32

#include <windows.h>
#include <shlobj.h>

#endif

using namespace OsintgramCXX;

namespace fs = std::filesystem;

struct CommandExecution {
    bool cmdFound;
    std::string msg;
    int rc;
};

bool timeMeasuringSystem = false;

void helpCmd() {
    if (OsintgramCXX::loadedLibraries.empty()) {
        std::cerr << "No commands have been added." << std::endl;
        std::cerr << "To add commands, load a native library with the use of 'commands.json' file," << std::endl;
        std::cerr << "and restart the application." << std::endl;
        std::cerr << "Safely exit out of the application by typing 'exit', 'quit' or 'close'" << std::endl;

        // shell beautifying
        CurrentThread_Sleep(70);
        return;
    }

    Terminal::println(std::cout, Terminal::TermColor::RED, "[General]", true);
    HelpPage gPage;
    gPage.setSpaceWidth(5);
    gPage.setStartSpaceWidth(2);
    gPage.addArg("help", "", "Shows this help page");
    gPage.addArg("exit", "", "Exits this application");
    Terminal::println(std::cout, Terminal::TermColor::CYAN, gPage.display(), true);

    for (const auto &cmdVar: OsintgramCXX::loadedLibraries) {
        const auto &item = cmdVar.second;
        Terminal::println(std::cout, Terminal::TermColor::RED, "[" + item.label + "]", true);

        HelpPage ePage;
        ePage.setStartSpaceWidth(2);
        ePage.setSpaceWidth(5);

        for (const auto &cmdEntry: item.commands)
            ePage.addArg(cmdEntry.cmd, "", cmdEntry.description);

        Terminal::println(std::cout, Terminal::TermColor::CYAN, ePage.display(), true);
    }
}

void forceStopShell(int) {
    OsintgramCXX::ShellFuckery::stopShell(true);
    OsintgramCXX::ShellFuckery::cleanup();
    std::cin.setstate(std::ios::badbit);
}

CommandExecution execCommand(const std::string &cmd, const std::vector<std::string> &args, const ShellEnvironment &env,
                             const std::string &cmdLine) {
    long long startTime;
    if (timeMeasuringSystem)
        startTime = OsintgramCXX::nanoTime();

    bool found = false;
    OsintgramCXX::C_CommandExec cmdExecHandle = nullptr;
    CommandExecution execReturn{};

    for (const auto &it: OsintgramCXX::loadedLibraries) {
        for (const auto &cmdEntry: it.second.commands) {
            if (cmd == cmdEntry.cmd) {
                found = true;
                cmdExecHandle = cmdEntry.execHandler;
                break;
            }
        }
    }

    if (!found) {
        execReturn.cmdFound = false;
        execReturn.msg = cmd + ": not found";
        execReturn.rc = 1;
        return execReturn;
    }

    if (cmdExecHandle == nullptr) {
        execReturn.cmdFound = false;
        execReturn.msg = cmd + ": handler for execution is missing";
        execReturn.rc = 1;

        return execReturn;
    }

    execReturn.cmdFound = true;

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
    for (const auto &it: OsintgramCXX::loadedLibraries) {
        std::thread t([handler = it.second, &cmdLine]() {
            if (handler.handler_onCmdExec != nullptr)
                handler.handler_onCmdExec(const_cast<char *>(cmdLine.c_str()));
        });
        t.detach();
    }

    try {
        execReturn.rc = cmdExecHandle(cmd.c_str(), args.size(), argv, env.size(), env_map);
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

    if (timeMeasuringSystem) {
        long long endTime = OsintgramCXX::nanoTime();
        long long duration = endTime - startTime;
        long long sec = duration / 1'000'000'000;
        long long millis = (duration / 1'000'000) % 1'000;

        std::cout << cmd << ": took " << sec << "." << std::setfill('0') << std::setw(3) << millis << " s" << std::endl;
    }

    return execReturn;
}

namespace OsintgramCXX::ShellFuckery {

    std::string PS1;
    bool running = false;
    [[maybe_unused]] bool shellInitialized = false;
    ShellEnvironment environment;
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

            if (opt[0] == "ENABLETIMEMEASURINGSYSTEM") {
                timeMeasuringSystem = opt[1] == "true" || opt[1] == "enabled" || opt[1] == "yes" || opt[1] == "1";
                return;
            }

            environment[opt[0]] = opt[1];
        } else {
            auto it = environment.find(worker);
            if (it == environment.end()) {
                Terminal::println(std::cerr, Terminal::TermColor::RED, worker + " (not found)", true);
                return;
            }

            Terminal::print(std::cout, Terminal::TermColor::BLUE, worker, true);
            std::cout << " => ";
            Terminal::print(std::cout, Terminal::TermColor::CYAN, it->second, true);
            std::cout << std::endl;
        }
    }

    void cmd() {
#ifdef __linux__
        signal(SIGINT, forceStopShell);
        signal(SIGTERM, forceStopShell);
        signal(SIGABRT, forceStopShell);
#endif

        std::string line;

        while (running) {
            try {
                std::cout << PS1;
                if (!std::getline(std::cin, line)) {
                    std::cerr << "exit initiated" << std::endl;
                    running = false;
                    break;
                }

                line = TrimString(line);

                // forgotten lines (would SegFault otherwise)
                if (line.empty())
                    continue;

                if (line[0] == '&') {
                    modEnviron(line);
                    continue;
                }

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

                CommandExecution ret = execCommand(cmdLine[0], cmdArgs, environment, line);
                if (!ret.cmdFound) {
                    std::cerr << ret.msg << std::endl;
                    CurrentThread_Sleep(70);

                    continue;
                }

                if (ret.rc != 0) {
                    std::cerr << cmdLine[0] << ": exit code " << ret.rc << std::endl;
                    CurrentThread_Sleep(70);
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
#if defined(__linux__)
            pthread_kill(shellThread.native_handle(), SIGABRT);
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
        shellInitialized = false;
    }

}