#ifndef OSINTGRAMCXX_SHELL_HPP
#define OSINTGRAMCXX_SHELL_HPP

#include "ShellEnv.hpp"

namespace OsintgramCXX::ShellFuckery {

    extern std::string PS1;
    extern bool running;
    [[maybe_unused]] extern bool shellInitialized;
    extern ShellEnvironment environment;
    extern std::vector<ShellCommand> shellCommands;

    void initializeShell();

    void launchShell();

    void stopShell(bool forceStop);

    void cleanup();

}

#endif //OSINTGRAMCXX_SHELL_HPP
