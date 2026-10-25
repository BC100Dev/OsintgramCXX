#ifndef OSINTGRAM_USERHOME_HPP
#define OSINTGRAM_USERHOME_HPP

#include <dev_tools/commons/Utils.hpp>

using namespace DevTools;

namespace User {

    extern HomeDirConfig homeConf;

#ifdef __linux__
    extern bool knownElevationMethodUsed;
    extern uid_t realUserId;
    extern std::string realUserName;
#endif

    void PopulateUserInfo();

}

#endif //OSINTGRAM_USERHOME_HPP
