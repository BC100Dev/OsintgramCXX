#ifndef OSINTGRAMCXX_SHELLENV_HPP
#define OSINTGRAMCXX_SHELLENV_HPP

#include <string>
#include <functional>
#include <stdexcept>
#include <map>

namespace OsintgramCXX {

    using ShellEnvironment = std::map<std::string, std::string>;

    // params: cmdName, argc, argv, envc, env_map
    using C_CommandExec = std::function<int(const char*, int, char **, int, char **)>;

    using C_OnLoadEntry = std::function<void()>;

    using C_OnExitEntry = std::function<void()>;

    // params: cmdLine
    using C_OnCommandExecutionStart = std::function<void(char *)>;

    // params: cmdLine, rc, id, stream
    using C_OnCommandExecutionFinish = std::function<void(char *, int, int, char*)>;

    struct ShellLibEntry {
        std::string cmd;
        std::string description;
        C_CommandExec execHandler;
    };

    class ShellException : public std::runtime_error {
    public:
        explicit ShellException(const std::string &message) : std::runtime_error(message) {}
    };

}

#endif //OSINTGRAMCXX_SHELLENV_HPP
