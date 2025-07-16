#include "../Commands.h"

#include <iostream>

int cmd_handle(const char *cmd, int argc, char **argv, int envc, char **env_map) {
    std::cout << "Testing command for: " << cmd << std::endl;
    return 0;
}