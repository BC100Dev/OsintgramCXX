#include "TermuxCheck.hpp"

#ifdef __ANDROID__
#include <fstream>

bool IsTermuxRunning() {
    if (getuid() < 10000)
        return false;

    const char* termuxPid = std::getenv("TERMUX_APP_PID");
    if (termuxPid == nullptr)
        return false;

    std::string cmdlinePath = "/proc/" + std::string(termuxPid) + "/cmdline";

    std::ifstream cmdline(cmdlinePath);
    if (!cmdline.is_open())
        return false;

    std::string processName;
    std::getline(cmdline, processName, '\0');

    return processName == "com.termux";
}
#endif
