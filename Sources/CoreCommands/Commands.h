#ifndef OSINTGRAMCXX_COMMANDS_H
#define OSINTGRAMCXX_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

int cmd_handle(const char* cmd, int argc, char **argv, int envc, char **env_map);

#ifdef __cplusplus
};
#endif

#endif //OSINTGRAMCXX_COMMANDS_H
