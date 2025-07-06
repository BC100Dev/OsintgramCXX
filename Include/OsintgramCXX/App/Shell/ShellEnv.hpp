#ifndef OSINTGRAMCXX_SHELLENV_HPP
#define OSINTGRAMCXX_SHELLENV_HPP

#include <string>
#include <functional>
#include <stdexcept>
#include <map>

namespace OsintgramCXX {

    using ShellEnvironment = std::map<std::string, std::string>;

    using C_CommandExec = std::function<int(const char*, int, char **, int, char **)>;

    using C_OnLoadEntry = std::function<int()>;

    using C_OnExitEntry = std::function<int()>;

    using C_OnCommandExecute = std::function<void(char *)>;

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
