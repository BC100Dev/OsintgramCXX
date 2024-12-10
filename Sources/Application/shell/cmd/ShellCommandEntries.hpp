#ifndef OSINTGRAMCXX_SHELLCOMMANDENTRIES_HPP
#define OSINTGRAMCXX_SHELLCOMMANDENTRIES_HPP

#include <OsintgramCXX/App/Shell/ShellEnv.hpp>

using namespace OsintgramCXX;

namespace AppShell {

    class ICommandEntry {
    public:
        virtual ~ICommandEntry() = default;
        virtual int execute(const std::vector<std::string> &, const ShellEnvironment &) = 0;
        virtual std::string helpCmd() const = 0;
    };

}

namespace AppShell::Commands {

    namespace EmptyNs {
        int CommandEntry(const std::vector<std::string> &, const ShellEnvironment &);
        std::string CommandHelp();
    }

    namespace CacheManager {
        int CommandEntry(const std::vector<std::string> &, const ShellEnvironment &);
        std::string CommandHelp();
    }

    namespace UserManager {
        int CommandEntry(const std::vector<std::string> &, const ShellEnvironment &);
        std::string CommandHelp();
    }

    namespace SessionManager {
        int CommandEntry(const std::vector<std::string> &, const ShellEnvironment &);
        std::string CommandHelp();
    }

    namespace SettingsControl {
        int CommandEntry(const std::vector<std::string> &, const ShellEnvironment &);
        std::string CommandHelp();
    }

    std::vector<ShellCommand> importCommands();

}

namespace AppShell::NetCommands {

    namespace FollowingsCmd {
        int CommandEntry(const std::vector<std::string> &, const ShellEnvironment &);
        std::string CommandHelp();
    }

    namespace FollowersCmd {
        int CommandEntry(const std::vector<std::string> &, const ShellEnvironment &);
        std::string CommandHelp();
    }

}

#endif //OSINTGRAMCXX_SHELLCOMMANDENTRIES_HPP
