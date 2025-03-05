#include <OsintgramCXX/App/Defaults.hpp>

#include <climits>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

std::string GetExecutablePath() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    fs::path exePath = std::string(buffer);
    return exePath.string();
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    fs::path exePath = std::string(result, (count > 0) ? count : 0);
    return exePath.string();
#endif
}

std::string ConstructFromRoot(const std::string& rootDir, const std::string& dirName) {
    std::stringstream sstream;
    sstream << rootDir;

#ifdef _WIN32
    sstream << "\\" << dirName;
#endif

#ifdef __linux__
    sstream << "/" << dirName;
#endif

    return sstream.str();
}

namespace OsintgramCXX {

    bool suppressWarnings = false;
    std::vector<Target> targetList;

    std::string GetRootDirectory() {
        fs::path exePath = GetExecutablePath();
        // needed to remove the executable name
        exePath = exePath.parent_path();
        return exePath.string();
    }

    std::string GetShareDirectory() {
        std::string dir;

#ifdef __linux__
        dir = "share";
#elif _WIN32
        dir = "Contents";
#endif

        return ConstructFromRoot(GetRootDirectory(), dir);
    }

    std::string GetLibraryDirectory() {
        std::string dir;

#ifdef __linux__
        dir = "lib";
#elif _WIN32
        dir = "Libraries";
#endif

        return ConstructFromRoot(GetRootDirectory(), dir);
    }

}