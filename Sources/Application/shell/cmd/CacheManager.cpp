#include "ShellCommandEntries.hpp"

using namespace OsintgramCXX;

int AppShell::Commands::CacheManager::CommandEntry(
        const std::vector<std::string> &args, const OsintgramCXX::ShellEnvironment &env) {
    return 0;
}

std::string AppShell::Commands::CacheManager::CommandHelp() {
    return "Ss"
           "ss";
}