#include <AppCommons/Utils.hpp>

namespace OsintgramCXX {

    bool StringContains(const std::string& str, const std::string& val) {
        return str.find(val) != std::string::npos;
    }

    std::string TrimString(const std::string &str) {
        size_t start = str.find_first_not_of(" \t\n");
        size_t end = str.find_last_not_of(" \t\n");

        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    std::map<int, std::string> SplitString(const std::string& str, const std::string& delim, int limit) {
        std::map<int, std::string> tokens;
        size_t startIndex = 0;
        size_t delimPos = 0;
        int tokenIndex = 0;

        while ((delimPos = str.find(delim, startIndex)) != std::string::npos) {
            tokens[tokenIndex++] = str.substr(startIndex, delimPos - startIndex);
            startIndex = delimPos + delim.length();

            if (limit > 0 && tokenIndex >= limit - 1) {
                break;
            }
        }

        tokens[tokenIndex] = str.substr(startIndex);

        return tokens;
    }

    std::map<int, std::string> SplitString(const std::string &str, const std::string &delim) {
        return SplitString(str, delim, 0);
    }

    bool StartsWith(const std::string &str, const std::string &prefix) {
        if (prefix.size() > str.size())
            return false;

        return std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    bool EndsWith(const std::string &str, const std::string &suffix) {
        if (suffix.size() > str.size())
            return false;

        return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

    // don't forget to pull up that Regex skill without googling it! (evil laughter)
    std::string ReplaceAll(const std::string& str, const std::string& pattern, const std::string& replacement) {
        try {
            std::regex regexPattern(pattern);
            return std::regex_replace(str, regexPattern, replacement);
        } catch (const std::regex_error& ex) {
            std::cerr << "Utils.hpp# ReplaceAll - Regex Error: " << ex.what() << std::endl;
            return str;
        }
    }

    std::string ReplaceFirst(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = result.find(from);
        if (pos != std::string::npos)
            result.replace(pos, from.length(), to);

        return result;
    }

    std::string Replace(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }

        return result;
    }

    bool IsAdmin() {
        bool result = false;
#ifdef __linux__
        result = (getuid() == 0 || geteuid() == 0) && getgid() == 0;
#endif

#ifdef _WIN32
        HANDLE hToken = nullptr;
        TOKEN_ELEVATION elevation;
        DWORD dwSize;

        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
                result = elevation.TokenIsElevated;

            CloseHandle(hToken);
            hToken = nullptr;
        }
#endif

        return result;
    }

    std::string GetHostname() {
        std::string result = "unknown-host";

#ifdef __linux__
        struct utsname buffer{};
        if (uname(&buffer) == 0)
            result = std::string(buffer.nodename);
#endif

#ifdef _WIN32
        char hostname[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(hostname) / sizeof(hostname[0]);
        if (GetComputerNameA(hostname, &size))
            result = std::string(hostname);
#endif

        return result;
    }

    std::string CurrentWorkingDirectory() {
        std::string result;

#ifdef __linux__
        char buff[PATH_MAX];
        if (getcwd(buff, PATH_MAX) != nullptr)
            result = std::string(buff);
#endif

#ifdef _WIN32
        char buff[MAX_PATH];
        if (_getcwd(buff, MAX_PATH) != nullptr)
            result = std::string(buff);
#endif

        return result;
    }

    std::string CurrentUsername() {
        std::string result;

#ifdef __linux__
        struct passwd *pw = getpwuid(getuid());
        result = pw ? std::string(pw->pw_name) : "nobody";
#endif

#ifdef _WIN32
        char username[256];
        DWORD username_len = sizeof(username) / sizeof(username[0]);

        if (GetUserNameExA(NameSamCompatible, username, &username_len))
            result = std::string(username);
#endif

        return result;
    }

}