#ifndef OSINTGRAMCXX_SHELL_HPP
#define OSINTGRAMCXX_SHELL_HPP

#include <map>
#include <string>
#include <functional>
#include <stdexcept>
#include <vector>
#include <thread>

#define ENFORCE_MANUAL_COMMAND_ENTRY_WARNING true
#define ENFORCE_MANUAL_COMMAND_HELPCMD_WARNING true

#define SHELL_APPLICATION_NAME "OsintgramCXX"

namespace Application {
    class AppShell;

    using ShellEnvironment = std::map<std::string, std::string>;
    using CommandLineInputArgs = std::vector<std::string>;

    struct CommandImpl {
        std::string name;
        std::string description;
        std::function<int(const CommandLineInputArgs& /* args */, const ShellEnvironment& /* env */)> handle;
    };

    // params: cmdName, argc, argv, envc, env_map
    using C_CommandExec = std::function<int(const char*, int, char**, int, char**)>;

    using CommandHelperFinderFn = std::function<bool(const std::string& /*cmd*/)>;
    using CommandHelperCallbackFn = std::function<int(const std::string& /*cmd*/,
                                                       const CommandLineInputArgs&,
                                                       const ShellEnvironment&)>;
    using CommandHelperListingFn = std::function<std::string()>;

    struct ShellLibEntry {
        std::string cmd;
        std::string description;
        C_CommandExec execHandler;
    };

    class ShellException : public std::runtime_error {
    public:
        explicit ShellException(const std::string& message) : std::runtime_error(message) {
        }
    };

    class AppShell {
    public:
        AppShell() {
            init_shell();
        }

        void Start();

        bool IsRunning() const;

        void Stop(bool forceStop);

        void Stop() {
            Stop(false);
        }

        void AddCommand(const CommandImpl& cmd);

        void SetCommandFallbackHandler(const CommandHelperFinderFn& finderFn,
                                              const CommandHelperCallbackFn& callbackFn,
                                              const CommandHelperListingFn& listingFn) const;

        void SetEnv(const std::string& key, const std::string& val);

        std::string GetEnv(const std::string& key);

    private:
        struct CommandExecution {
            bool cmdFound;
            std::string contents;
            std::string msg;
            int rc;
        };

        void init_shell();

        void launch_shell();

        void handle_stdin();

        void cleanup();

        CommandExecution run_cmd(const std::string& cmd,
                                 const std::vector<std::string>& args,
                                 const ShellEnvironment& env,
                                 const std::string& cmdLine);

        void chEnvMapTable(const std::string& line);

        void helpCmd();

        [[deprecated]] static void reloadCmd();


        std::string m_prompt;
        bool m_customCommandEntryWarning = true;
        bool m_initialized = false;
        bool m_startOnCurrentThread = false;
        bool m_running = false;
        bool m_alreadyForceStopped = false;
        ShellEnvironment m_environment;
        bool m_timeMeasuringSystem = false;
        std::vector<CommandImpl> m_cmdList;

        std::thread m_shellThread;
    };

    AppShell& GetShellInstance();
}

#endif //OSINTGRAMCXX_SHELL_HPP
