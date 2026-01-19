#ifndef OSINTGRAMCXX_COMMANDS_H
#define OSINTGRAMCXX_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CMD_HANDLE_EXPORTED
#define CMD_HANDLE_EXPORTED

int cmd_handle(const char* cmd, int argc, char** argv, int envc, char** env_map);

    // This symbol can be used to get a quick entry, while debugging / testing your own plugin / mod.
    // By default, this SHOULD BE disabled and not implemented for security reasons.
    // Secure your test entry, in case
int runlib_entry_test(int argc, char** argv);
#endif

#ifdef __cplusplus
}
#endif

#endif //OSINTGRAMCXX_COMMANDS_H
