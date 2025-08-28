#ifndef OSINTGRAMCXX_MODHANDLES_HPP
#define OSINTGRAMCXX_MODHANDLES_HPP

#include <vector>
#include <string>
#include <optional>

#include <OsintgramCXX/App/Shell/ShellEnv.hpp>

#ifdef __linux__

#include <dlfcn.h>

#endif

namespace OsintgramCXX {

    //// main command executor
    //// args: cmd, argc, argv, env_size, env_map
    //typedef int (*C_CommandExec)(int, char **, int, char **);

    //typedef int (*C_OnLoadEntry)();

    //typedef int (*C_OnExitEntry)();
    //typedef void (*C_OnCommandExecute)(char *); // args: cmdLine

    struct LibraryEntry {
        std::string label;
        long long id;
        std::optional<std::string> author;
        std::optional<std::string> description;
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
