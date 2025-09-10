#include <iostream>

extern "C" {

    int plugin_handle_start() {
        return 0;
    }

    int plugin_handle_stop() {
        return 0;
    }

    void plugin_cmd_exec_start(char *command) {
    }

    void plugin_cmd_exec_finish(char* command, int rc, char* stream) {
    }

}