#include <AppShell/Shell.hpp>
#include <OsintgramCXX/App/ModHandles.hpp>

#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/commons/Terminal.hpp>
#include <dev_tools/commons/Tools.hpp>
#include <dev_tools/commons/HelpPage.hpp>

#include <sstream>
#include <thread>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <ranges>
#include <functional>

// #include <openssl/x509.h> (???)

#include "StdCapture.hpp"

#ifdef __linux__

#include <csignal>

#elif _WIN32

#include <windows.h>
#include <shlobj.h>

#endif

using namespace Application;
using namespace OsintgramCXX;
using namespace DevTools;

#define NFCommandExec std::function<int(const CommandLineInputArgs& /* args */, const ShellEnvironment& /* env */)>
#define CMD_VARIANT_LIST C_CommandExec, NFCommandExec

namespace fs = std::filesystem;

namespace Application {
    static std::optional<AppShell> instance = std::nullopt;
    sig_atomic_t g_forceStop = 0;

    bool g_hasAddedExternalCommand = false;

    void HandleShellForceClose(int);

    void AppShell::AddCommand(const CommandImpl& cmd) {
        for (const auto& val : loadedLibraries | std::views::values) {
            for (auto& data = val; const auto& exCmd : data.commands) {
                if (exCmd.cmd == cmd.name)
                    throw ShellException("Command by the name of \"" + cmd.name + "\" exists");
            }
        }

        for (const auto& val : m_cmdList) {
            if (val.name == cmd.name)
                throw ShellException("Command by the name of \"" + cmd.name + "\" exists");
        }

#if ENFORCE_MANUAL_COMMAND_ENTRY_WARNING
        if (m_customCommandEntryWarning && !g_hasAddedExternalCommand) {
            Terminal::println(std::cout, Terminal::TermColor::RED, "!!! WARNING !!!", false);
            std::cout << "A custom command has been added by an external library via a command / hook!" << std::endl;
            std::cout << "If you suspect of any malicious libraries, immediately shut down the app" << std::endl;
            Terminal::println(std::cout, Terminal::TermColor::RED, "and remove any suspicious entries!", true);

            g_hasAddedExternalCommand = true;
        }
#endif

        m_cmdList.emplace_back(cmd);
    }

    void AppShell::cleanup() {
        std::cin.setstate(std::ios::badbit);

#ifdef __linux__
        close(STDIN_FILENO);
#endif

#ifdef _WIN32
        CloseHandle(GetStdHandle(STD_INPUT_HANDLE));
#endif

        m_prompt.clear();
        m_environment.clear();
        m_initialized = false;
        m_running = false;
    }

    void AppShell::init_shell() {
        m_alreadyForceStopped = false;

        std::string user = CurrentUsername();
        std::string sCwd = CurrentWorkingDirectory();
        fs::path cwd = fs::current_path();

        std::stringstream strStream;
        strStream << "[" << user << " % " << SHELL_APPLICATION_NAME << "] >> ";

        m_prompt = strStream.str();
        m_initialized = true;
        m_customCommandEntryWarning = true;
    }

    bool AppShell::IsRunning() const {
        return m_running;
    }

    void AppShell::launch_shell() {
        if (!m_initialized) {
            std::cerr << "Shell not initialized" << std::endl;
            return;
        }

        m_running = true;

        try {
            std::thread shellThread([&] {
                handle_stdin();
            });
            m_shellThread = std::move(shellThread);
            m_shellThread.join();
        } catch (std::exception& ex) {
            if (std::string(ex.what()) != "Invalid argument") {
                std::cerr << "Shell Thread Error (Shell.cpp): " << ex.what() << std::endl;
                Stop(false);
            }
        }
    }

    void AppShell::Start() {
        launch_shell();
    }

    void AppShell::Stop(bool forceStop) {
        m_running = false;
        cleanup();

        if (forceStop && !m_alreadyForceStopped) {
#if defined(__linux__)
            pthread_kill(m_shellThread.native_handle(), SIGABRT);
#elif defined(_WIN32)
            TerminateThread(reinterpret_cast<HANDLE>(m_shellThread.native_handle()), 1);
#endif

            m_alreadyForceStopped = true;
        }
    }

    void AppShell::SetEnv(const std::string& key, const std::string& val) {
        if (key == "PS1") {
            m_prompt = val;
            return;
        }

        if (ToLowercase(key) == ToLowercase("EnableTimeMeasuringSystem")) {
            m_timeMeasuringSystem = val == "true" || val == "enabled" || val == "yes" || val == "1";
            return;
        }

        m_environment[key] = val;
    }

    void AppShell::chEnvMapTable(const std::string& line) {
        std::string worker = TrimString(line);
        if (worker[0] == '&')
            worker = worker.substr(1);

        if (StringContains(line, "=")) {
            std::vector<std::string> opt = SplitString(worker, "=", 2);
            opt[0] = TrimString(opt[0]);
            opt[1] = TrimString(opt[1]);

            SetEnv(opt[0], opt[1]);
        } else {
            auto it = m_environment.find(worker);
            if (it == m_environment.end()) {
                std::cerr << worker << " (not found)" << std::endl;
                return;
            }

            std::cout << worker << " => " << it->second << std::endl;
        }
    }

    void AppShell::handle_stdin() {
        std::string line;
        std::string multiLineCmd;
        bool isMultiline = false;

#ifdef __linux__
        signal(SIGABRT, HandleShellForceClose);
        signal(SIGINT, HandleShellForceClose);
        signal(SIGSTOP, HandleShellForceClose);
        signal(SIGTERM, HandleShellForceClose);
#endif

        while (m_running && !g_forceStop) {
            try {
                std::cout << (isMultiline ? ">>> " : m_prompt);
                if (!std::getline(std::cin, line)) {
                    std::cerr << "exit initiated" << std::endl;
                    m_running = false;
                    break;
                }

                line = TrimString(line);

                if (line.empty())
                    continue;

                if (line[0] == '&') {
                    chEnvMapTable(line);
                    continue;
                }

                if (line.ends_with("\\")) {
                    multiLineCmd += line.substr(0, line.size() - 1);

                    if (!multiLineCmd.ends_with(' '))
                        multiLineCmd += ' ';

                    if (!isMultiline)
                        isMultiline = true;

                    continue;
                }

                if (isMultiline)
                    multiLineCmd += line;

                std::vector<std::string> cmdLine = TranslateStrToCmdline(isMultiline ? multiLineCmd : line);
                std::vector<std::string> cmdArgs;

                if (cmdLine.size() > 1) {
                    for (size_t i = 1; i < cmdLine.size(); i++)
                        cmdArgs.push_back(cmdLine[i]);
                }

                if (cmdLine[0] == "exit" || cmdLine[0] == "quit" || cmdLine[0] == "close") {
                    Stop(false);
                    return;
                }

                if (cmdLine[0] == "help") {
                    helpCmd();
                    continue;
                }

                isMultiline = false;
                multiLineCmd = "";

                CommandExecution ret = run_cmd(cmdLine[0], cmdArgs, m_environment, line);
                if (!ret.cmdFound) {
                    std::cerr << ret.msg << std::endl;
                    threadSleep(70);

                    continue;
                }

                if (ret.rc != 0) {
                    std::cerr << cmdLine[0] << ": exit code " << ret.rc << std::endl;
                    threadSleep(70);
                }
            } catch (const std::exception& ex) {
                std::cerr << "ShellError: " << ex.what() << std::endl;
            }
        }
    }

    void AppShell::helpCmd() {
        if (loadedLibraries.empty() && m_cmdList.empty()) {
            std::cerr << "No commands have been added." << std::endl;
            std::cerr << "To add commands, fetch a commands.json present and restart the instance." << std::endl;
            std::cerr << "Exiting this application can be done by typing \"exit\"." << std::endl;

            // shell beautifying at its finest
            threadSleep(70);
            return;
        }

        std::cout << "[Global] (built-in)" << std::endl;
        HelpPage gPage;
        gPage.setSpaceWidth(5);
        gPage.setStartSpaceWidth(2);
        gPage.addArg("help", std::nullopt, "Shows this help page");
        gPage.addArg("exit", std::nullopt, "Exits this application");
        gPage.display(std::cout);
        std::cout << std::endl;

        for (const auto& val : loadedLibraries) {
            auto& item = val.second;

            Terminal::print(std::cout, Terminal::TermColor::YELLOW, "[" + item.label + "]", true);
            if (item.author.has_value())
                Terminal::print(std::cout, Terminal::TermColor::PURPLE, " (" + item.author.value() + ")", true);

            std::cout << "\n";

            HelpPage ePage;
            ePage.setSpaceWidth(5);
            ePage.setStartSpaceWidth(2);

            for (const auto& cmdEntry : item.commands)
                ePage.addArg(cmdEntry.cmd, std::nullopt, cmdEntry.description);

            Terminal::println(std::cout, Terminal::TermColor::CYAN, ePage.display(), true);
        }

        if (!m_cmdList.empty()) {
#if ENFORCE_MANUAL_COMMAND_HELPCMD_WARNING
            Terminal::println(std::cout, Terminal::TermColor::RED, "!!! MANUALLY ADDED COMMANDS !!!", false);
            std::cout << "Make sure that the native libraries you have added are trusted." << std::endl;
            Terminal::println(std::cout,
                              Terminal::TermColor::RED,
                              "Manually added libraries by external libraries can be malicious, could have special triggers and should be reconsidered before executing.\n",
                              true);
#endif

            std::cout << "[Global] (built-in)" << std::endl;
            HelpPage mPage;
            mPage.setSpaceWidth(5);
            mPage.setStartSpaceWidth(2);
            for (const auto& manCmds : m_cmdList)
                mPage.addArg(manCmds.name, std::nullopt, manCmds.description);

            mPage.display(std::cout);
        }
    }

    [[deprecated]] void AppShell::reloadCmd() {
        // it was meant to be able to reload all "commands.json" configs,
        // but this would mean that I would need to move another part of the codebase,
        // specifically from "Application" to another library or something. peak laziness haha
        // idk why you reading this p.o.s, but if you do, just be mindful that I was lazy
        // and that I wanted a few additional methods.
    }

    AppShell::CommandExecution AppShell::run_cmd(const std::string& cmd,
                                                 const std::vector<std::string>& args,
                                                 const ShellEnvironment& env,
                                                 const std::string& cmdLine) {
        long long startTime = nanoTime();

        bool found = false;
        std::variant<CMD_VARIANT_LIST> cmdExecHandlerVar;
        CommandExecution execReturn{};

        for (const auto& val : loadedLibraries | std::views::values) {
            for (const auto& cmdEntry : val.commands) {
                if (cmd == cmdEntry.cmd) {
                    found = true;
                    cmdExecHandlerVar = cmdEntry.execHandler;
                    break;
                }
            }
        }

        if (!found) {
            for (const auto& manCmd : m_cmdList) {
                if (cmd == manCmd.name) {
                    found = true;
                    cmdExecHandlerVar = manCmd.handle;
                    break;
                }
            }
        }

        if (!found) {
            execReturn.cmdFound = false;
            execReturn.msg = cmd + ": command not found";
            execReturn.rc = 1;
            return execReturn;
        }

        if (std::holds_alternative<C_CommandExec>(cmdExecHandlerVar)) {
            if (auto qt = std::get<C_CommandExec>(cmdExecHandlerVar); !qt) {
                execReturn.cmdFound = false;
                execReturn.msg = cmd + ": handler for execution is not defined";
                execReturn.rc = 1;

                return execReturn;
            }
        }

        execReturn.cmdFound = true;

        // start the listeners for "OnCommandExec"
        for (const auto& val : loadedLibraries | std::views::values) {
            if (LibraryEntry entry = val; entry.handler_onCmdExecStart != nullptr) {
                std::thread t([handler = val, &cmdLine] {
                    handler.handler_onCmdExecStart(const_cast<char*>(cmdLine.c_str()));
                });
                t.detach();
            }
        }

        StdCapture cap;

        if (std::holds_alternative<C_CommandExec>(cmdExecHandlerVar)) {
            auto argv = new char*[args.size()];
            for (size_t i = 0; i < args.size(); i++) {
                argv[i] = strdup(args[i].c_str());
            }

            auto env_map = new char*[env.size()];
            int index = 0;
            for (const auto& [key, value] : env) {
                std::string entry = key;
                entry.append("=").append(value);

                env_map[index++] = strdup(entry.c_str());
            }

            try {
                execReturn.rc = std::get<C_CommandExec>(cmdExecHandlerVar)
                    (cmd.c_str(), args.size(), argv, env.size(), env_map);
            } catch (const std::runtime_error& err) {
                std::cerr << "Runtime error occurred, while executing \"" << cmd << "\": " << err.what() << std::endl;
            } catch (const std::exception& err) {
                std::cerr << "Error occurred, while executing \"" << cmd << "\": " << err.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown error occurred, while executing \"" << cmd << "\"" << std::endl;
            }

            for (int i = 0; i < env.size(); i++) {
                free(env_map[i]);
            }
            delete[] env_map;

            for (int i = 0; i < args.size(); i++) {
                free(argv[i]);
            }

            delete[] argv;
        } else if (std::holds_alternative<NFCommandExec>(cmdExecHandlerVar)) {
            try {
                execReturn.rc = std::get<NFCommandExec>(cmdExecHandlerVar)(args, env);
            } catch (const std::runtime_error& err) {
                std::cerr << "Runtime error occurred, while executing \"" << cmd << "\": " << err.what() << std::endl;
            } catch (const std::exception& err) {
                std::cerr << "Error occurred, while executing \"" << cmd << "\": " << err.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown error occurred, while executing \"" << cmd << "\"" << std::endl;
            }
        }

        for (const auto& val : loadedLibraries | std::views::values) {
            if (LibraryEntry entry = val; entry.handler_onCmdExecFinish != nullptr) {
                std::thread t([handler = entry, &cmdLine, &execReturn, output = cap.str()] {
                    handler.handler_onCmdExecFinish(const_cast<char*>(cmdLine.c_str()),
                                                    execReturn.rc,
                                                    handler.id,
                                                    const_cast<char*>(output.c_str()));
                });
                t.join();
            }
        }

        if (m_timeMeasuringSystem) {
            long long endTime = nanoTime();
            long long duration = endTime - startTime;
            long long sec = duration / 1'000'000'000;
            long long millis = (duration / 1'000'000) % 1'000;

            std::cout << cmd << ": took " << sec << "." << std::setfill('0') << std::setw(3) << millis << " s" <<
                std::endl;
        }

        return execReturn;
    }

    AppShell& GetShellInstance() {
        if (instance == std::nullopt)
            instance = AppShell();

        return instance.value();
    }

    void HandleShellForceClose(int) {
        g_forceStop = 1;

        AppShell& shell = GetShellInstance();
        shell.Stop(true);
    }
}
