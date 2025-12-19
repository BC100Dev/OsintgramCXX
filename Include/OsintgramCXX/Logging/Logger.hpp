#ifndef OSINTGRAMCXX_LOGGER_HPP
#define OSINTGRAMCXX_LOGGER_HPP

#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;

namespace Logging {
    enum class LogLevel {
        INFO = 0xE0,
        WARNING = 0xE1,
        ERROR = 0xE2,
        FATAL = 0xE3,
        DEBUG = 0xE4,
        VERBOSE = 0xE5,
        TRACE = 0xE6,
        CRITICAL = 0xE7,
        GENERIC = 0xE8,
        USER = 0xE9
    };

    class Logger {
    public:
        explicit Logger(const std::string& name);
        ~Logger() = default;

        void log(const std::string& message);
        void log(const LogLevel& lvl, const std::string& message);

        template <typename... Args>
        void logf(const std::string& fmt, Args&&... args) {
            log(std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void logf(const LogLevel& lvl, const std::string& fmt, Args&&... args) {
            log(std::format(fmt, std::forward<Args>(args)...), lvl);
        }

        std::string& loggerName() {
            return logName;
        }

        void flush();

        void close();

    private:
        std::ofstream mainFileStream;
        std::string logName;

        // logger configuration (needs to be configured from the constructor)
        bool timestamp = true;
        std::string timestampFormat = "{Day}-{Month}-{Year}_{Hour}:{Minute}:{Second}";
        LogLevel defLogLevel = LogLevel::INFO;
        std::string logFormat = "[%Level%] %Timestamp% -- %Data%";
        bool appendContentUponStart = true;
        bool autoFlush = true;
    };

    std::string formatTimestamp(const std::string& in, const std::string& format);

    std::string formatLogData(const std::string& in, const std::string& format);

    inline std::string formatLogLevel(const LogLevel& lvl) {
        switch (lvl) {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::VERBOSE:
            return "VERBOSE";
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::CRITICAL:
            return "CRITICAL";
        case LogLevel::GENERIC:
            return "GENERIC";
        case LogLevel::USER:
            return "USER";
        default: // unlikely to happen, but default value
            return formatLogLevel(LogLevel::DEBUG);
        }
    }

    namespace Instances {
        std::shared_ptr<Logger> getApplicationLoggingInstance();

        std::shared_ptr<Logger> getNetworkLoggingInstance();

        std::shared_ptr<Logger> getSecureLoggerInstance();

    }
}

#endif //OSINTGRAMCXX_LOGGER_HPP
