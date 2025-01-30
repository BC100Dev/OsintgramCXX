#ifndef OSINTGRAMCXX_SHELLENV_HPP
#define OSINTGRAMCXX_SHELLENV_HPP

#include <string>
#include <functional>
#include <stdexcept>
#include <map>
#include <OsintgramCXX/App/ModHandles.hpp>

namespace OsintgramCXX {

    using ShellEnvironment = std::map<std::string, std::string>;

    struct ShellCommand {

        std::string commandName;
        std::string quickHelpStr;
        ModHandles::ShellLibEntry libEntry;

    };

    class ShellException : public std::runtime_error {
    public:
        explicit ShellException(const std::string &message) : std::runtime_error(message) {}
    };

}

#endif //OSINTGRAMCXX_SHELLENV_HPP
