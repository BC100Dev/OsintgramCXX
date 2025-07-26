#include <Commands.h>

#include <FuncConstructor.hpp>
#include "FuncHeaders.hpp"

#include <iostream>

int cmd_handle(const char *cmd, int argc, char **argv, int envc, char **env_map) {
    CoreFunctions::funcMap["userctl"] = user_func;
    CoreFunctions::funcMap["cachectl"] = cache_func;
    CoreFunctions::funcMap["sessionctl"] = session_func;
    CoreFunctions::funcMap["net"] = net_func;
    CoreFunctions::funcMap["dsi-updater"] = dsi_func;

    if (CoreFunctions::funcMap.find(std::string(cmd)) == CoreFunctions::funcMap.end()) {
        std::cerr << cmd << ": command not found" << std::endl;
        return 1;
    }

    FuncParams params = MakeFuncParams(argc, argv, envc, env_map);
    return CoreFunctions::funcMap[cmd](params.args, params.env);
}