#ifndef OSINTGRAMCXX_SHELLENV_HPP
#define OSINTGRAMCXX_SHELLENV_HPP

#include <string>
#include <functional>

namespace OsintgramCXX {

    struct ShellEnvironment {

        std::string name;
        std::string value;

    };

    struct ShellCommand {

        std::string commandName;
        std::function<int(const std::vector<std::string>&, const std::vector<ShellEnvironment>&)> executionCommand;
        std::function<std::string(const std::vector<std::string>&)> helpCommand;
        std::string quickHelpStr;

    };

    extern std::vector<ShellEnvironment> shellEnvMap;
    extern std::vector<ShellCommand> shellCommandMap;

}

#endif //OSINTGRAMCXX_SHELLENV_HPP
