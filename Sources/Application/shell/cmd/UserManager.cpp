#include "ShellCommandEntries.hpp"

using namespace OsintgramCXX;

int AppShell::Commands::UserManager::CommandEntry(
        const std::vector<std::string> &args, const OsintgramCXX::ShellEnvironment &env) {
    return 0;
}

std::string AppShell::Commands::UserManager::CommandHelp() {
    return "Ss"
           "ss";
}