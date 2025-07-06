#include "Commands.h"

#include <iostream>

int cmd_handle(const char* cmd, int argc, char **argv, int envc, char **env_map) {
    std::cout << "haha very funny " << cmd << std::endl;
    return 0;
}