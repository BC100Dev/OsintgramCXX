#ifndef OSINTGRAMCXX_MODHANDLES_HPP
#define OSINTGRAMCXX_MODHANDLES_HPP

#include <vector>
#include <string>
#include <optional>

#ifdef __linux__

#include <dlfcn.h>

#endif

namespace ModHandles {

    //// main command executor
    //// args: argc, argv, env_size, env_map
    //typedef int (*C_CommandExec)(int, char **, int, char **);

    //typedef int (*C_OnLoadEntry)();

    //typedef int (*C_OnExitEntry)();
    //typedef void (*C_OnCommandExecute)(char *); // args: cmdLine

    using C_CommandExec = std::function<int(int, char **, int, char **)>;

    using C_OnLoadEntry = std::function<int()>;

    using C_OnExitEntry = std::function<int()>;

    using C_OnCommandExecute = std::function<void(char *)>;

    struct ShellLibEntry {
        std::string cmd;
        std::string description;
        C_CommandExec execHandler;
    };

    struct LibraryEntry {
        C_OnLoadEntry handler_onLoad;
        C_OnExitEntry handler_onExit;
        C_OnCommandExecute handler_onCmdExec;
        std::vector<ShellLibEntry> commands;
    };

    // uses:
    // void*                   native library handle
    // vector<ShellLibEntry>   executable commands
    extern std::map<void *, LibraryEntry> loadedLibraries;

}

#endif //OSINTGRAMCXX_MODHANDLES_HPP
