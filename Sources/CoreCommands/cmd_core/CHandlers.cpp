#include <iostream>

extern "C" {

    int plugin_handle_start() {
        return 0;
    }

    int plugin_handle_stop() {
        return 0;
    }

    void plugin_handle_exec(char *command) {
    }

}