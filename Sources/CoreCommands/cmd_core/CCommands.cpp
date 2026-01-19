#include <Commands.h>

#include <FuncConstructor.hpp>
#include "FuncHeaders.hpp"

#include <iostream>

int cmd_handle(const char *cmd, int argc, char **argv, int envc, char **env_map) {
    // the shell usually does the check beforehand. This check is here, in case something within the code went wrong
    if (!CoreFunctions::funcMap.contains(cmd)) {
        std::cerr << cmd << ": command integration failed" << std::endl;
        return 1;
    }

    auto [args, env] = MakeFuncParams(argc, argv, envc, env_map);
    return CoreFunctions::funcMap[cmd](args, env);
}

int runlib_entry_point(int argc, char** argv) {
    return 0;
}