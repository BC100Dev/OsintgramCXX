#include <AppShell/Shell.hpp>

#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/commons/Tools.hpp>
#include <dev_tools/commons/HelpPage.hpp>

#include <sstream>
#include <thread>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <functional>
#include <variant>

#include "StdCapture.hpp"

#ifdef __linux__

#include <csignal>

#elif _WIN32

#include <windows.h>

#endif

using namespace Application;
using namespace DevTools;

#define NFCommandExec std::function<int(const CommandLineInputArgs& /* args */, const ShellEnvironment& /* env */)>
#define CMD_VARIANT_LIST C_CommandExec, NFCommandExec

namespace fs = std::filesystem;

namespace Application {
    static std::optional<AppShell> instance = std::nullopt;
    sig_atomic_t g_forceStop = 0;

    void HandleShellForceClose(int);

    struct AdditionalCommandHelperP {
        CommandHelperFinderFn finderFunc;
        CommandHelperCallbackFn callbackFunc;
        CommandHelperListingFn listingFunc;
    };

    std::vector<CommandExecutionListener> execListeners;
    std::vector<AdditionalCommandHelperP> additCommandHelpers;

    void AppShell::AddCommand(const CommandImpl& cmd) {
        for (const auto& val : m_cmdList) {
            if (val.name == cmd.name)
                throw ShellException("Command by the name of \"" + cmd.name + "\" exists");
        }

        for (const auto& addit : additCommandHelpers) {
            if (addit.finderFunc(cmd.name))
                throw ShellException("Command by the name of \"" + cmd.name + "\" is declared by a module");
        }

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

        std::stringstream strStream;
        strStream << "[" << CurrentUsername() << " % " << SHELL_APPLICATION_NAME << "] >> ";

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
            m_timeMeasuringSystem = val == "true" || val == "enabled" || val[0] == 'y' || val == "1";
            return;
        }

        m_environment[key] = val;
    }

    void AppShell::chEnvMapTable(const std::string& line) {
        std::string worker = TrimString(line);
        if (worker.empty())
            return;

        if (worker[0] == '&')
            worker = worker.substr(1);

        // another processing after potential '&' removal
        if (worker.empty())
            return;

        if (size_t eqp = worker.find('='); eqp != std::string::npos) {
            std::string k = TrimString(worker.substr(0, eqp));
            std::string v = TrimString(worker.substr(eqp + 1));

            if (k.starts_with('\"') || k.starts_with('\''))
                k = k.substr(1);

            if (k.ends_with('\"') || k.ends_with('\''))
                k = k.substr(0, k.size() - 1);

            if (k.empty()) {
                std::cerr << "An environment variable's key name cannot be empty." << std::endl;
                return;
            }

            if (v.starts_with('\"') || v.starts_with('\''))
                v = v.substr(1);

            if (v.ends_with('\"') || v.ends_with('\''))
                v = v.substr(0, v.size() - 1);

            SetEnv(k, v);
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

        auto ResetMultiline = [&] {
            isMultiline = false;
            multiLineCmd = "";
        };

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

                if (line.ends_with('\\')) {
                    multiLineCmd += line.substr(0, line.size() - 1);

                    if (!multiLineCmd.ends_with(' '))
                        multiLineCmd += ' ';

                    if (!isMultiline)
                        isMultiline = true;

                    continue;
                }

                if (isMultiline)
                    multiLineCmd += line;

                std::vector<std::string> cmdLine{};
                try {
                    cmdLine = TranslateStrToCmdline(isMultiline ? multiLineCmd : line);
                } catch (const std::runtime_error& e) {
                    if (std::string(e.what()).contains("unbalanced quotes")) {
                        std::cerr << "Unbalanced quotes detected." << std::endl;
                        std::cerr << "Please retype your command again." << std::endl;
                        ResetMultiline();
                        threadSleep(70);
                        continue;
                    }
                } catch (...) {
                    std::cerr << "An issue occurred, while translating cmdline" << std::endl;
                    ResetMultiline();
                    threadSleep(70);
                    continue;
                }

                std::vector<std::string> cmdArgs;

                if (cmdLine.size() > 1) {
                    for (size_t i = 1; i < cmdLine.size(); i++)
                        cmdArgs.push_back(cmdLine[i]);
                }

                if (cmdLine[0] == "exit" || cmdLine[0] == "quit" || cmdLine[0] == "close") {
                    ResetMultiline();
                    Stop(false);
                    return;
                }

                if (cmdLine[0] == "help") {
                    helpCmd();
                    ResetMultiline();
                    continue;
                }

                CommandExecution ret = run_cmd(isMultiline ? multiLineCmd : line, cmdLine[0], cmdArgs, m_environment);
                if (!ret.cmdFound) {
                    std::cerr << ret.msg << std::endl;
                    threadSleep(70);

                    ResetMultiline();
                    continue;
                }

                ResetMultiline();
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
        std::cout << "[Global]" << std::endl;
        HelpPage gPage;
        gPage.setSpaceWidth(5);
        gPage.setStartSpaceWidth(2);
        gPage.addArg("help", std::nullopt, "Shows this help page");
        gPage.addArg("exit", std::nullopt, "Exits this application");
        gPage.display(std::cout);
        std::cout << std::endl;

        if (!m_cmdList.empty()) {
            std::cout << "[Commands]" << std::endl;
            HelpPage mPage;
            mPage.setSpaceWidth(5);
            mPage.setStartSpaceWidth(2);
            for (const auto& manCmds : m_cmdList)
                mPage.addArg(manCmds.name, std::nullopt, manCmds.description);

            mPage.display(std::cout);
        }

        for (const auto& addit : additCommandHelpers) {
            std::string listingData = addit.listingFunc();
            listingData = TrimString(listingData);

            if (!listingData.empty())
                std::cout << listingData << "\n" << std::endl;
        }
    }

    AppShell::CommandExecution AppShell::run_cmd(const std::string& cmdline,
                                                 const std::string& cmd,
                                                 const std::vector<std::string>& args,
                                                 const ShellEnvironment& env) {
        long long startTime = nanoTime();

        bool found = false;
        bool usingCallback = false;
        std::variant<CMD_VARIANT_LIST> cmdExecHandlerVar;
        CommandExecution execReturn{};

        for (const auto& manCmd : m_cmdList) {
            if (cmd == manCmd.name) {
                found = true;
                cmdExecHandlerVar = manCmd.handle;
                break;
            }
        }

        std::optional<CommandHelperCallbackFn> callbackFn = std::nullopt;
        for (const auto& addit : additCommandHelpers) {
            if (addit.finderFunc(cmd)) {
                found = true;
                usingCallback = true;
                callbackFn = addit.callbackFunc;
            }
        }

        if (!found) {
            execReturn.cmdFound = false;
            execReturn.msg = cmd + ": command not found";
            execReturn.rc = 1;
            return execReturn;
        }

        execReturn.cmdFound = true;

        for (const auto& execLis : execListeners) {
            std::thread th([&] {
                try {
                    execLis(ExecutionType::PRE_EXEC, cmdline, std::nullopt);
                } catch (...) {
                }
            });
            th.detach();
        }

        StdCapture scap;

        if (usingCallback && callbackFn.has_value())
            execReturn.rc = callbackFn.value()(cmd, args, env);
        else if (std::holds_alternative<NFCommandExec>(cmdExecHandlerVar)) {
            try {
                execReturn.rc = std::get<NFCommandExec>(cmdExecHandlerVar)(args, env);
            } catch (const std::runtime_error& err) {
                std::cerr << "Runtime error occurred, while executing \"" << cmd << "\": " << err.what() << std::endl;
                execReturn.rc = -105;
            } catch (const std::exception& err) {
                std::cerr << "Error occurred, while executing \"" << cmd << "\": " << err.what() << std::endl;
                execReturn.rc = -106;
            } catch (...) {
                std::cerr << "Unknown error occurred, while executing \"" << cmd << "\"" << std::endl;
                execReturn.rc = -107;
            }
        }

        for (const auto& execLis : execListeners) {
            std::thread th([&] {
                try {
                    execLis(ExecutionType::POST_EXEC, cmdline, ExecutionData{
                                .returnCode = execReturn.rc,
                                .data = scap.str()
                            });
                } catch (...) {
                }
            });
            th.detach();
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

    void AppShell::AddCommandFallbackHandler(const CommandFallbackContent& data) const {
        AdditionalCommandHelperP p;
        p.callbackFunc = data.callbackFn;
        p.listingFunc = data.listingFn;
        p.finderFunc = data.finderFn;

        additCommandHelpers.emplace_back(p);
    }

    void AppShell::AddCommandExecutionListener(const CommandExecutionListener& listener) const {
        execListeners.emplace_back(listener);
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

    std::string AppShell::GetEnv(const std::string& key) {
        if (!m_environment.contains(key))
            throw ShellException("Key \"" + key + "\" not found");

        return m_environment[key];
    }
}
