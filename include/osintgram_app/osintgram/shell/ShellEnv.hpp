#ifndef OSINTGRAMCXX_SHELLENV_HPP
#define OSINTGRAMCXX_SHELLENV_HPP

#include <string>
#include <functional>
#include <stdexcept>
#include <map>

namespace OsintgramCXX {

    using ShellEnvironment = std::map<std::string, std::string>;

    struct ShellCommand {

        std::string commandName;
        std::function<int(const std::vector<std::string> &, const ShellEnvironment &)> executionCommand;
        std::function<std::string(const std::vector<std::string> &)> helpCommand;
        std::string quickHelpStr;

    };

    class ShellException : public std::runtime_error {
    public:
        explicit ShellException(const std::string &message) : std::runtime_error(message) {}
    };

}

#endif //OSINTGRAMCXX_SHELLENV_HPP
