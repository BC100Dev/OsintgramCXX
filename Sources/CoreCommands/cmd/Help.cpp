#include "Commands.h"

#include <iostream>

int help_itself(int argc, char **argv, int envc, char **env_map) {
    std::cout << "Help" << std::endl;

    return 0;
}