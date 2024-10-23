#ifndef OSINTGRAMCXX_UTILS_HPP
#define OSINTGRAMCXX_UTILS_HPP

#include <map>
#include <string>
#include <sstream>
#include <regex>
#include <iostream>

#ifdef __linux__

#include <unistd.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <climits>

#endif

#ifdef _WIN32
#define SECURITY_WIN32

#include <windows.h>
#include <secext.h>
#include <direct.h>
#endif

namespace OsintgramCXX {

    bool StringContains(const std::string& str, const std::string& val);

    std::string TrimString(const std::string &str);

    std::vector<std::string> SplitString(const std::string& str, const std::string& delim, int limit);

    std::vector<std::string> SplitString(const std::string &str, const std::string &delim);

    bool StartsWith(const std::string &str, const std::string &prefix);

    bool EndsWith(const std::string &str, const std::string &suffix);

    // don't forget to pull up that Regex skill without googling it! (evil laughter)
    std::string ReplaceAll(const std::string& str, const std::string& pattern, const std::string& replacement);

    std::string ReplaceFirst(const std::string& str, const std::string& from, const std::string& to);

    std::string Replace(const std::string& str, const std::string& from, const std::string& to);

    bool IsAdmin();

    std::string GetHostname();

    std::string CurrentWorkingDirectory();

    std::string CurrentUsername();

    long RandomLong(long min, long max);

    double RandomDouble(double min, double max);

    float RandomFloat(float min, float max);

    int RandomInteger(int min, int max);

    void CurrentThread_Sleep(long duration);

    std::vector<char> Pause(const std::string& prompt, const ssize_t& count);
    std::vector<char> Pause(const std::string& prompt);
    std::vector<char> Pause();

}

#endif //OSINTGRAMCXX_UTILS_HPP
