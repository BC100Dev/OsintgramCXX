#include "ShellCommandEntries.hpp"

using namespace OsintgramCXX;

int AppShell::Commands::SessionManager::CommandEntry(
        const std::vector<std::string> &args, const OsintgramCXX::ShellEnvironment &env) {
    return 0;
}

std::string AppShell::Commands::SessionManager::CommandHelp() {
    return "Ss"
           "ss";
}