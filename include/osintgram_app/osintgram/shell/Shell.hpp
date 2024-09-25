#ifndef OSINTGRAMCXX_SHELL_HPP
#define OSINTGRAMCXX_SHELL_HPP

#include "ShellEnv.hpp"

namespace OsintgramCXX::Shell {

    extern std::string PS1;
    extern bool running;
    extern bool shellInitialized;
    extern ShellEnvironment environment;
    extern std::vector<ShellCommand> shellCommands;

    void initializeShell();

    void launchShell();

    void stopShell();

}

#endif //OSINTGRAMCXX_SHELL_HPP
