#include <OsintgramCXX/App/WineDetect.hpp>
#include <string>

#ifdef _WIN32
#include <windows.h>

typedef void (CDECL *wine_get_host_version_t)(const char **sysname, const char **release);
#endif

using namespace OsintgramCXX;

bool Wine::WineExecution() {
    bool result = false;

#ifdef _WIN32
    HMODULE hMod = GetModuleHandleA("ntdll.dll");
    if (!hMod)
        return result;

    FARPROC wine_get_version = GetProcAddress(hMod, "wine_get_version");
    if (wine_get_version)
        result = true;
#endif

    return result;
}

Wine::Host Wine::WineHost() {
    Host result{NONE, ""};

#ifdef _WIN32
    HMODULE hMod = GetModuleHandleA("ntdll.dll");
    if (!hMod)
        return result;

    auto hostVer = (wine_get_host_version_t) GetProcAddress(hMod, "wine_get_host_version");
    if (hostVer) {
        const char* sysName = nullptr;
        const char* release = nullptr;

        hostVer(&sysName, &release);

        std::string sys(sysName);
        if (sys == "Linux")
            result.platform = LINUX;
        else if (sys == "Darwin")
            result.platform = MAC_OS;
        else
            result.platform = UNKNOWN;

        result.version = std::string(release);
    }
#endif

    return result;
}

std::string Wine::HostPlatformToString(Wine::HostPlatform platform) {
    switch (platform) {
        case Wine::LINUX:
            return "Linux";
        case Wine::MAC_OS:
            return "macOS";
        case Wine::NONE:
            return "None";
        case Wine::UNKNOWN:
            return "Unknown";
        default:
            return "what";
    }
}