#ifndef OSINTGRAMCXX_LOGGER_HPP
#define OSINTGRAMCXX_LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>

#ifdef __linux__
#include <unistd.h>
#endif

#define OsintgramCXX_DefaultPaths_WinX64 "{{user.home}}\\AppData\\Local\\BC100Dev\\OsintgramCXX"
#define OsintgramCXX_DefaultPaths_Linux "{{user.home}}/.config/bc100dev/OsintgramCXX"
#define OsintgramCXX_DefaultPaths_MacOS "{{user.home}}/Library/Application Support/BC100Dev/OsintgramCXX"

class TargetClass {
public:
    // do nothing for now

private:
    // do nothing for now
};

struct Target {
    std::string name;
    TargetClass targetClass;
};

namespace fs = std::filesystem;

namespace OsintgramCXX {

    enum LogLevel {
        INFO,
        WARN,
        SEVERE,
        ERR,
        VERBOSE,
        DEBUG,
        SUCCESS
    };

    class Logger {
    public:
        ~Logger() {
            if (fileStream.is_open()) {
                fileStream.close();
            }
        }

        void log(LogLevel level, const std::string& message, bool onDisplay, const std::exception& exception) {
            if (!fileStream.is_open())
                throw std::runtime_error("Log File Write Error");

            std::string excWhat = exception.what();
            std::string sLog = FormatLog(level, message, excWhat);

            if (onDisplay)
                std::cout << sLog << std::endl;

            fileStream << sLog << std::endl << std::endl;
        }

        void log(LogLevel level, const std::string& message, bool onDisplay) {
            log(level, message, onDisplay, std::runtime_error(""));
        }

        void init(const std::string& _filePath) {
            if (!OpenLogFile(_filePath))
                throw std::runtime_error("Log File Open Error");
        }

        void close() {
            fileStream.close();
        }

        std::string GetFilePath() {
            return filePath;
        }

    private:
        std::ofstream fileStream;
        std::string filePath;

        static std::string LevelToString(LogLevel lvl) {
            switch (lvl) {
                case INFO:
                    return "INFO";
                case WARN:
                    return "WARNING";
                case SEVERE:
                    return "SEVERE";
                case ERR:
                    return "ERROR";
                case VERBOSE:
                    return "VERBOSE";
                case DEBUG:
                    return "DEBUG";
                case SUCCESS:
                    return "SUCCESS";
            }

            return "UNKNWN";
        }

        static std::string FormatLog(const LogLevel& lvl, const std::string& message, const std::string& excWhat) {
            auto now = std::chrono::system_clock::now();
            std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
            std::tm* localTime = std::localtime(&currentTime);

            int hour = localTime->tm_hour,
                    minute = localTime->tm_min,
                    second = localTime->tm_sec,
                    day = localTime->tm_mday,
                    month = localTime->tm_mon + 1,
                    year = localTime->tm_year + 1900;

            std::stringstream _stream;
            _stream << "[" << LevelToString(lvl) << "]["
                    << std::setw(2) << std::setfill('0') << hour << ":"
                    << std::setw(2) << std::setfill('0') << minute << ":"
                    << std::setw(2) << std::setfill('0') << second << ", "
                    << std::setw(2) << std::setfill('0') << day << "/"
                    << std::setw(2) << std::setfill('0') << month << "/"
                    << year << "] " << message;

            if (!excWhat.empty())
                _stream << "\n\t" << excWhat;

            return _stream.str();
        }

        bool OpenLogFile(const std::string& _filePath) {
            fileStream = std::ofstream(_filePath);
            bool ret = fileStream.is_open();

            filePath = _filePath;
            return ret;
        }
    };

    extern Logger applicationLogger;
    extern Logger networkLogger;

    extern bool suppressWarnings;
    extern std::vector<Target> targetList;

    std::string GetRootDirectory();
    //std::string GetBinaryDirectory();
    std::string GetShareDirectory();
    std::string GetLibraryDirectory();

}

#endif //OSINTGRAMCXX_LOGGER_HPP
