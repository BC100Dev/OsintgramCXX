#ifndef OSINTGRAMCXX_AppPROPS_HPP
#define OSINTGRAMCXX_AppPROPS_HPP

#include <string>
#include "../Commons/Utils.hpp"

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

#define OsintgramCXX_Developer "BC100Dev"
#define OsintgramCXX_DeveloperInfo "contact@bc100dev.net | https://bc100dev.net/about"

std::string TEXT_BLOCK() {
    std::string block = R"(
   *@@@%%%@@&,     ,&@@%%%&@@@.  |    (@@&%%%@@@        @&.          ,&@.
  #@&      ,@@(   #@%.           |  .&@(      \@\      .@@,          *@@,
 .&@.       /@%  .@@/   .*****,  |  ,@@            @@@@@@@@@@@% .%@@@@@@@@@@%.
  #@&      ,@@/   %@#       @@|  |   &@/      /@/      .@@,          *@@,
   ,@@@@&@@@&.     *@@@@@@@@@@.  |   \#@@@@&@@@/       .@@,          *@@,     )";

    if (!block.empty() && block[0] == '\n') {
        block.erase(0,1);
    }

    return block;
}

#endif //OSINTGRAMCXX_APPPROPS_HPP
