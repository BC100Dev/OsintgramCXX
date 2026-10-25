#ifndef OSINTGRAM_PATHS_HPP
#define OSINTGRAM_PATHS_HPP

#include <cstdlib>
#include <filesystem>
#include <dev_tools/commons/Utils.hpp>

#ifdef __linux__
#include <dev_tools/commons/User.hpp>
#include <optional>
#endif

namespace fs = std::filesystem;
using namespace DevTools;

namespace App::Paths {

#ifdef _WIN32
    inline fs::path AppDataRoaming() {
        if (const char* APPDATA = std::getenv("APPDATA"); APPDATA)
            return fs::path(APPDATA);

        return UserHomeDirectory() / "AppData" / "Roaming";
    }

    inline fs::path AppDataLocal() {
        if (const char* LOCALAPPDATA = std::getenv("LOCALAPPDATA"); LOCALAPPDATA)
            return fs::path(LOCALAPPDATA);

        return UserHomeDirectory() / "AppData" / "Local";
    }

    inline fs::path ProgramData() {
        if (const char* PROGRAMDATA = std::getenv("PROGRAMDATA"); PROGRAMDATA)
            return fs::path(PROGRAMDATA);

        if (const char* SYSTEMDRIVE = std::getenv("SYSTEMDRIVE"); SYSTEMDRIVE)
            return fs::path(SYSTEMDRIVE) / "ProgramData";

        return fs::path("C:\\ProgramData");
    }

    inline fs::path StartupDirectory(bool systemWide) {
        fs::path prefix = systemWide ? AppDataRoaming() : ProgramData();
        return prefix / "Microsoft" / "Windows" / "Start Menu" / "Programs" / "Startup";
    }

    inline fs::path SystemDrive() {
        if (const char* SYSTEMDRIVE = std::getenv("SYSTEMDRIVE"); SYSTEMDRIVE)
            return fs::path(SYSTEMDRIVE);

        return fs::path("C:");
    }
#endif

    inline fs::path UserLocal() {
#ifdef _WIN32
        return AppDataLocal();
#else
        return UserHomeDirectory(User::homeConf) / ".local" / "share";
#endif
    }

    inline fs::path UserConfig() {
#ifdef _WIN32
        return AppDataRoaming();
#else
        return UserHomeDirectory(User::homeConf) / ".config";
#endif
    }

#ifdef __linux__
    inline fs::path UserCache() {
        return UserHomeDirectory(User::homeConf) / ".cache";
    }

    inline std::optional<fs::path> UserBinDir() {
        if (fs::exists(UserLocal() / "bin"))
            return fs::path(UserLocal() / "bin");

        if (fs::exists(UserHomeDirectory(User::homeConf) / ".bin"))
            return UserHomeDirectory(User::homeConf) / ".bin";

        if (fs::exists(UserHomeDirectory(User::homeConf) / "bin"))
            return UserHomeDirectory(User::homeConf) / "bin";

        return std::nullopt;
    }

    inline std::optional<fs::path> UserLibDir() {
        if (fs::exists(UserLocal() / "lib"))
            return fs::path(UserLocal() / "lib");

        if (fs::exists(UserHomeDirectory(User::homeConf) / ".lib"))
            return UserHomeDirectory(User::homeConf) / ".lib";

        if (fs::exists(UserHomeDirectory(User::homeConf) / "lib"))
            return UserHomeDirectory(User::homeConf) / "lib";

        return std::nullopt;
    }
#endif

}

#endif //OSINTGRAM_PATHS_HPP
