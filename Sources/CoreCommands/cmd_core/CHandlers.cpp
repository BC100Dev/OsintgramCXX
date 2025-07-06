#include <iostream>

extern "C" {

    /*
     *                 {
                    "handler": "OnLoad",
                    "symbol": "plugin_handle_start"
                },
                {
                    "handler": "OnStop",
                    "symbol": "plugin_handle_stop"
                },
                {
                    "handler": "OnCommandExec",
                    "symbol": "plugin_handle_exec"
                }
     */

    int plugin_handle_start() {
        std::cout << "Plugin Loaded" << std::endl;
        return 0;
    }

    int plugin_handle_stop() {
        return 0;
    }

    void plugin_handle_exec(char *command) {
    }

}