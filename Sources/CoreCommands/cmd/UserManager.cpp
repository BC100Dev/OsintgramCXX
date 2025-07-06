#include "Commands.h"

#include <iostream>

int user_manager(const char* cmd, int argc, char **argv, int envc, char **env_map) {
    std::cout << "Calling from User Manager" << std::endl;
    return 0;
}