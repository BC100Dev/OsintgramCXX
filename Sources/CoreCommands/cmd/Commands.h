#ifndef OSINTGRAMCXX_COMMANDS_H
#define OSINTGRAMCXX_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

int cache_manager(int argc, char **argv, int envc, char **env_map);

int help_itself(int argc, char **argv, int envc, char **env_map);

int session_manager(int argc, char **argv, int envc, char **env_map);

int user_manager(const char* cmd, int argc, char **argv, int envc, char **env_map);

int cmd_handle(const char* cmd, int argc, char **argv, int envc, char **env_map);

#ifdef __cplusplus
};
#endif

#endif //OSINTGRAMCXX_COMMANDS_H
