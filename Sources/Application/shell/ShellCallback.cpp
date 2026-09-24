#include "ShellCallback.hpp"

#include <iostream>
#include <ranges>
#include <cstring>
#include <sstream>

#include "../ModInit.hpp"

#include <dev_tools/commons/HelpPage.hpp>
#include <OsintgramCXX/App/ModHandles.hpp>

using namespace OsintgramCXX;

bool ShellHelpers_Finder(const std::string& cmd) {
    for (const auto& val : OsintgramCXX::loadedLibraries | std::views::values) {
        for (const auto& it : val.commands) {
            if (it.cmd == cmd)
                return true;
        }
    }

    return false;
}

int ShellHelpers_Executor(const std::string& cmd, const CommandLineInputArgs& args, const ShellEnvironment& env) {
    std::optional<C_CommandExec> cmdExecHandler = std::nullopt;

    for (const auto& lib : OsintgramCXX::loadedLibraries | std::views::values) {
        for (const auto& _cmd : lib.commands) {
            if (_cmd.cmd == cmd && _cmd.execHandler)
                cmdExecHandler = _cmd.execHandler;
        }
    }

    if (!cmdExecHandler) {
        std::cerr << cmd << ": exec handler not defined" << std::endl;
        return 1;
    }

    int rc = 0;
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
        rc = cmdExecHandler.value()(cmd.c_str(), args.size(), argv, env.size(), env_map);
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
    return rc;
}

std::string ShellHelpers_Listings() {
    std::ostringstream oss;
    for (const auto& lib : OsintgramCXX::loadedLibraries | std::views::values) {
        oss << "[" << lib.label;

        if (lib.author.has_value())
            oss << " (" << lib.author.value() << ")";

        oss << "]\n";

        HelpPage hp;
        hp.setDescSeparator("");
        hp.setSpaceWidth(5);
        hp.setStartSpaceWidth(2);

        for (const auto& [cmd, description, execHandler] : lib.commands) {
            if (execHandler != nullptr)
                hp.addArg(cmd, std::nullopt, description);
        }

        oss << hp.display() << "\n";
    }

    return oss.str() + "\n";
}

void ShellHelpers_CommandExecListener(const ExecutionType& exType,
                                      const std::string& cmdline,
                                      std::optional<int> retCode,
                                      const std::optional<std::string>& data) {
    for (const auto& val : loadedLibraries | std::views::values) {
        if (exType == ExecutionType::PRE_EXEC && val.handler_onCmdExecStart != nullptr) {
            std::thread th([&] {
                val.handler_onCmdExecStart(const_cast<char*>(cmdline.c_str()));
            });
            th.detach();
        }

        if (exType == ExecutionType::POST_EXEC && val.handler_onCmdExecFinish != nullptr) {
            std::thread th([&] {
                val.handler_onCmdExecFinish(const_cast<char*>(cmdline.c_str()),
                    retCode.has_value() ? retCode.value() : -16,
                    const_cast<char*>(std::string(data.has_value() ? data.value() : "").c_str()));
            });
            th.join();
        }
    }
}

void OSINT_IncludeShellCallback(const AppShell& shell) {
    shell.SetCommandFallbackHandler({
                                        .finderFn = ShellHelpers_Finder,
                                        .callbackFn = ShellHelpers_Executor,
                                        .listingFn = ShellHelpers_Listings
                                    }, true);
    shell.SetCommandExecutionListener(ShellHelpers_CommandExecListener);
}
