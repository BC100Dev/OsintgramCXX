#include <OsintgramCXX/Commons/Utils.hpp>
#include <random>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <fstream>

#define PAUSE_PROMPT_DEFAULT "Press any key to continue..."

#ifdef _WIN32

#include <windows.h>
#include <conio.h>

#elif __linux__

#include <termios.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <fcntl.h>

#endif

namespace OsintgramCXX {

    std::string ToLowercase(const std::string &str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        return result;
    }

    std::string ToUppercase(const std::string &str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        return result;
    }

    bool StringContains(const std::string &str, const std::string &val) {
        return str.find(val) != std::string::npos;
    }

    std::string TrimString(const std::string &str) {
        return TrimString(str, " \t\r\n");
    }

    std::string TrimString(const std::string &str, const std::string &chars) {
        size_t start = str.find_first_not_of(chars);
        size_t end = str.find_last_not_of(chars);
        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    std::vector<std::string> SplitString(const std::string &str, const std::string &delim, int limit) {
        std::vector<std::string> tokens;
        size_t startIndex = 0;
        size_t delimPos = 0;
        int tokenIndex = 0;

        while ((delimPos = str.find(delim, startIndex)) != std::string::npos) {
            tokens.push_back(str.substr(startIndex, delimPos - startIndex));
            tokenIndex++;

            startIndex = delimPos + delim.length();

            if (limit > 0 && tokenIndex >= limit - 1)
                break;
        }

        tokens.push_back(str.substr(startIndex));

        return tokens;
    }

    std::vector<std::string> SplitString(const std::string &str, const std::string &delim) {
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
    std::string ReplaceAll(const std::string &str, const std::string &pattern, const std::string &replacement) {
        try {
            std::regex regexPattern(pattern);
            return std::regex_replace(str, regexPattern, replacement);
        } catch (const std::regex_error &ex) {
            std::cerr << "Utils.hpp# ReplaceAll - Regex Error: " << ex.what() << std::endl;
            return str;
        }
    }

    std::string ReplaceFirst(const std::string &str, const std::string &from, const std::string &to) {
        std::string result = str;
        size_t pos = result.find(from);
        if (pos != std::string::npos)
            result.replace(pos, from.length(), to);

        return result;
    }

    std::string Replace(const std::string &str, const std::string &from, const std::string &to) {
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
        char hostname[HOST_NAME_MAX];
        if (gethostname(hostname, sizeof(hostname)) == 0)
            result = std::string(hostname);
#endif

#ifdef _WIN32
        char hostname[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(hostname) / sizeof(hostname[0]);
        if (GetComputerNameA(hostname, &size))
            result = std::string(hostname);
#endif

        return result;
    }

    std::string UserDomain() {
        std::string result = "LOCALHOST|UNKNOWN-DOMAIN";

#ifdef __linux__
        result = GetHostname() + "/" + CurrentUsername();
#elif _WIN32
        char username[256];
        DWORD username_len = sizeof(username);

        if (GetUserNameExA(NameSamCompatible, username, &username_len))
            result = username;
#endif

        return result;
    }

    std::string CurrentWorkingDirectory() {
        // apparently, there was a simpler choice all along
        // who could have known?
        return std::filesystem::current_path().string();
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

        if (GetUserNameA(username, &username_len))
            result = std::string(username);
#endif

        return result;
    }

    long RandomLong(long min, long max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long> dis(min, max);
        return dis(gen);
    }

    long long RandomLLong(long long min, long long max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dis(min, max);
        return dis(gen);
    }

    unsigned long long RandomULLong(unsigned long long min, unsigned long long max) {
        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<unsigned long long> dis(min, max);
        return dis(gen);
    }

    double RandomDouble(double min, double max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_real_distribution<double> dis(min, max);
        return dis(gen);
    }

    float RandomFloat(float min, float max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    int RandomInteger(int min, int max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(min, max);
        return dis(gen);
    }

    long RandomSeed(long min, long max) {
        std::random_device rd;
        unsigned int seed = rd();

        std::mt19937_64 gen(seed);
        std::uniform_int_distribution<long> dis(min, max);

        long rnd = dis(gen);
        rnd *= RandomInteger(10000, 55000) + RandomLong(5000, 500000) + (RandomLong(500, 1000) * RandomLong(2, 100000))
                                                                        * 99999;
        return rnd;
    }

    void CurrentThread_Sleep(long duration) {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    }

    std::vector<char> Pause() {
        return Pause(PAUSE_PROMPT_DEFAULT, 1);
    }

    std::vector<char> Pause(const std::string &prompt) {
        return Pause(prompt, 1);
    }

    std::vector<char> Pause(const std::string &prompt, const ssize_t &count) {
        std::cout << prompt;
        std::vector<char> chArr(count);

#ifdef _WIN32
        for (size_t i = 0; i < count; ++i)
            chArr[i] = _getch();
#elif __linux__
        termios oldT{}, newT{};
        tcgetattr(STDIN_FILENO, &oldT);
        newT = oldT;
        newT.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newT);

        for (int i = 0; i < count; i++) {
            char _c;
            ssize_t nr = read(STDIN_FILENO, &_c, 1);
            if (nr == -1) {
                std::cerr << "Read Error" << std::endl;
                chArr.clear();
                break;
            }

            chArr[i] = _c;
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldT);
#endif

        return chArr;
    }

    void ExitProgram(int code) {
        std::exit(code);
    }

    void CreateFile(const std::string &path) {
        if (std::filesystem::exists(path))
            return;

#ifdef _WIN32
        HANDLE hFile = ::CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            std::string error;

            {
                std::stringstream ss;
                ss << "Could not create file, error 0x" << std::hex << GetLastError();

                error = ss.str();
            }

            throw std::runtime_error(error);
        }

        CloseHandle(hFile);
#else
        int fd = open(path.c_str(), O_CREAT, 0664);
        if (fd == -1) {
            fd = open(path.c_str(), O_WRONLY | O_CREAT, 0664);

            if (fd == -1)
                throw std::runtime_error("Could not create file, error " + std::to_string(errno));
        }

        close(fd);
#endif
    }

    std::string ExecutableDirectory() {
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        std::filesystem::path exePath = std::string(buffer);
        return exePath.string();
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        std::filesystem::path exePath = std::string(result, (count > 0) ? count : 0);
        return exePath.parent_path().string();
#endif
    }

}