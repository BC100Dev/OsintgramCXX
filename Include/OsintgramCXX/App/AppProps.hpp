#ifndef OSINTGRAMCXX_AppPROPS_HPP
#define OSINTGRAMCXX_AppPROPS_HPP

#include <string>
#include <format>
#include <dev_tools/commons/Utils.hpp>

#define OsintgramCXX_VersionName "v0.1"
#define OsintgramCXX_VersionCode 1

#ifdef _WIN32
#define OsintgramCXX_PlatformBuild "Windows"
#elif __linux__
#define OsintgramCXX_PlatformBuild "Linux"
#elif __APPLE__
// for the person that is still building against macOS, I despise you
// not really, but you get the point that it is not compatible, and you would have to write C++ code for macOS
#define OsintgramCXX_PlatformBuild "macOS"
#endif

inline std::string TEXT_BLOCK() {
    std::string block = R"(
   ___     _       _
  /___\___(_)_ __ | |_ __ _ _ __ __ _ _ __ ___
 //  // __| | '_ \| __/ _` | '__/ _` | '_ ` _ \
/ \_//\__ \ | | | | || (_| | | | (_| | | | | | |
\___/ |___/_|_| |_|\__\__, |_|  \__,_|_| |_| |_|
                      |___/                     )";

    if (!block.empty() && block[0] == '\n') {
        block.erase(0,1);
    }

    return block;
}

#endif //OSINTGRAMCXX_AppPROPS_HPP
