#include "ShellCommandEntries.hpp"

using namespace OsintgramCXX;

int AppShell::Commands::SettingsControl::CommandEntry(
        const std::vector<std::string> &args, const OsintgramCXX::ShellEnvironment &env) {
    return 0;
}

std::string AppShell::Commands::SettingsControl::CommandHelp() {
    return "Ss"
           "ss";
}