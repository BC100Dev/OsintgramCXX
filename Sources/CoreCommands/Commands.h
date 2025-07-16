#ifndef OSINTGRAMCXX_COMMANDS_H
#define OSINTGRAMCXX_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CMD_HANDLE_EXPORTED
#define CMD_HANDLE_EXPORTED

int cmd_handle(const char* cmd, int argc, char **argv, int envc, char **env_map);
#endif

#ifdef __cplusplus
};
#endif

#endif //OSINTGRAMCXX_COMMANDS_H
