#ifndef OSINTGRAMCXX_MODHANDLES_HPP
#define OSINTGRAMCXX_MODHANDLES_HPP

#include <vector>
#include <string>
#include <optional>

#include <AppShell/Shell.hpp>

#ifdef __linux__

#include <dlfcn.h>

#endif

namespace OsintgramCXX {

    //// main command executor
    //// args: cmd, argc, argv, env_size, env_map
    //typedef int (*C_CommandExec)(int, char **, int, char **);

    //typedef int (*C_OnLoadEntry)();

    //typedef int (*C_OnExitEntry)();
    //typedef void (*C_OnCommandExecutionStart)(char *); // args: cmdLine

    struct LibraryEntry {
        std::string label;
        int id;
        std::optional<std::string> author;
        std::optional<std::string> description;
        Application::C_OnLoadEntry handler_onLoad;
        Application::C_OnExitEntry handler_onExit;
        Application::C_OnCommandExecutionStart handler_onCmdExecStart;
        Application::C_OnCommandExecutionFinish handler_onCmdExecFinish;
        std::vector<Application::ShellLibEntry> commands;
    };

    // uses:
    // void*                        native library handle
    // std::vector<ShellLibEntry>   executable commands
    extern std::map<void *, LibraryEntry> loadedLibraries;

}

#endif //OSINTGRAMCXX_MODHANDLES_HPP
