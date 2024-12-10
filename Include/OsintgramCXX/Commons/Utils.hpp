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

#define MAX_PATH_LIMIT PATH_MAX

#endif

#ifdef _WIN32
#define SECURITY_WIN32

#include <windows.h>
#include <secext.h>
#include <direct.h>

#define MAX_PATH_LIMIT MAX_PATH
#endif

namespace OsintgramCXX {

    std::string ToLowercase(const std::string &str);

    std::string ToUppercase(const std::string &str);

    bool StringContains(const std::string &str, const std::string &val);

    std::string TrimString(const std::string &str);

    std::string TrimString(const std::string &str, const std::string &chars);

    std::vector<std::string> SplitString(const std::string &str, const std::string &delim, int limit);

    std::vector<std::string> SplitString(const std::string &str, const std::string &delim);

    bool StartsWith(const std::string &str, const std::string &prefix);

    bool EndsWith(const std::string &str, const std::string &suffix);

    // don't forget to pull up that Regex skill without googling it! (evil laughter)
    std::string ReplaceAll(const std::string &str, const std::string &pattern, const std::string &replacement);

    std::string ReplaceFirst(const std::string &str, const std::string &from, const std::string &to);

    std::string Replace(const std::string &str, const std::string &from, const std::string &to);

    bool IsAdmin();

    std::string GetHostname();

    std::string UserDomain();

    std::string CurrentWorkingDirectory();

    std::string CurrentUsername();

    long RandomLong(long min, long max);

    long long RandomLLong(long long min, long long max);

    unsigned long long RandomULLong(unsigned long long min, unsigned long long max);

    double RandomDouble(double min, double max);

    float RandomFloat(float min, float max);

    int RandomInteger(int min, int max);

    // this method, is what we call, a sacrificial lamp.
    // better hook this method up on a proper 64-bit CPU because we be generating high numbers.
    // I'd recommend at least sparing 512 MB for this project
    long RandomSeed(long min = 5, long max = 500);

    void CurrentThread_Sleep(long duration);

    std::vector<char> Pause(const std::string &prompt, const ssize_t &count);

    std::vector<char> Pause(const std::string &prompt);

    std::vector<char> Pause();

    void ExitProgram(int code);

}

#endif //OSINTGRAMCXX_UTILS_HPP
