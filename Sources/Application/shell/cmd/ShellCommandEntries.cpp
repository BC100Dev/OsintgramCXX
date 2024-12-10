#include "ShellCommandEntries.hpp"

using namespace AppShell::Commands;

std::vector<ShellCommand> AppShell::Commands::importCommands() {
    std::vector<ShellCommand> commands;

    // add entries here
    ShellCommand cacheMgr;
    cacheMgr.commandName = "cachectl";
    cacheMgr.executionCommand = CacheManager::CommandEntry;
    cacheMgr.helpCommand = CacheManager::CommandHelp;
    cacheMgr.quickHelpStr = "Manages OsintgramCXX and Instagram Cache (involves Cookies & Auth)";
    commands.push_back(cacheMgr);

    ShellCommand userMgr;
    userMgr.commandName = "userctl";
    userMgr.executionCommand = UserManager::CommandEntry;
    userMgr.helpCommand = UserManager::CommandHelp;
    userMgr.quickHelpStr = "Manages Instagram Accounts";
    commands.push_back(userMgr);

    ShellCommand sessionMgr;
    sessionMgr.commandName = "sessionctl";
    sessionMgr.executionCommand = SessionManager::CommandEntry;
    sessionMgr.helpCommand = SessionManager::CommandHelp;
    sessionMgr.quickHelpStr = "Manages Instagram Connection Sessions";
    commands.push_back(sessionMgr);

    ShellCommand settingsCtl;
    settingsCtl.commandName = "settingsctl";
    settingsCtl.executionCommand = SettingsControl::CommandEntry;
    settingsCtl.helpCommand = SettingsControl::CommandHelp;
    settingsCtl.quickHelpStr = "OsintgramCXX Settings Control";
    commands.push_back(settingsCtl);

    return commands;
}