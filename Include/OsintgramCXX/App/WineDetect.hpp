#ifndef OSINTGRAMCXX_WINEDETECT_HPP
#define OSINTGRAMCXX_WINEDETECT_HPP

#include <string>

namespace OsintgramCXX::Wine {

    enum HostPlatform {
        LINUX = 0,
        MAC_OS = 1,
        NONE = 2,
        UNKNOWN = 3
    };

    struct Host {

        HostPlatform platform;
        std::string version;

    };

    bool WineExecution();

    Host WineHost();

    std::string HostPlatformToString(HostPlatform platform);

}

#endif //OSINTGRAMCXX_WINEDETECT_HPP
