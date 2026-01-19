#include <Commands.h>

#include <iostream>

int cmd_handle(const char* cmd, int argc, char **argv, int envc, char **env_map) {
    std::cout << "Testing Instagram command for: " << cmd << std::endl;
    return 0;
}

int runlib_entry_point(int argc, char** argv) {
    return 0;
}