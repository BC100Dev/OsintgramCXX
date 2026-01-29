#include <iostream>
#include <exception>
#include <vector>
#include <thread>
#include <sstream>
#include <typeinfo>
#include <filesystem>
#include <csignal>

#include <OsintgramCXX/App/Shell/Shell.hpp>
#include <OsintgramCXX/App/AppProps.hpp>

#include <dev_tools/commons/HelpPage.hpp>
#include <dev_tools/commons/Terminal.hpp>
#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/logging/Logger.hpp>

#include "ModInit.hpp"
#include "AndroidCA.hpp"
#include "OsintgramCXX/App/WineDetect.hpp"
#include "settings/AppSettings.hpp"

#ifdef _WIN32

#include <windows.h>

#elif defined(__linux__)

#include <unistd.h>
#include <climits>
#include <cerrno>
#include <cstring>
#include <sys/mount.h>

#endif

#if defined(__linux__) && !defined(__ANDROID__)

#include <sys/capability.h>

#endif

namespace fs = std::filesystem;
using namespace OsintgramCXX;
using namespace DevTools;

std::string chrootPath;

std::map<std::string, std::string> defMap;

void WinSetColorMode() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

void exceptionHandler() {
    if (std::exception_ptr exPtr = std::current_exception()) {
        try {
            std::rethrow_exception(exPtr);
        } catch (const std::exception &ex) {
            std::ostringstream sw;
            sw << ex.what();

            Terminal::println(std::cerr, Terminal::TermColor::RED,
                              "The application has unexpectedly crashed. Cause of this error:", false);
            Terminal::println(std::cerr, Terminal::TermColor::RED, sw.str(), true);
        }
    } else
        Terminal::println(std::cerr, Terminal::TermColor::RED, "Unknown Termination captured", true);

    std::raise(SIGSEGV);
}

void usage() {
    std::string cmd = ".";
    std::string executablePath;
    const char *pathDelim;

#ifdef _WIN32
    pathDelim = "\\";

    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    executablePath = std::string(buffer);
#else
    pathDelim = "/";

    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

    if (len == -1) {
        perror("usage() => readlink failed");
        return;
    }

    buffer[len] = '\0';
    executablePath = std::string(buffer);
#endif

    cmd.append(pathDelim);

    if (!executablePath.empty()) {
        fs::path filePath(executablePath);

        // in favor of Osintgram4j being discontinued
        if (std::string fileName = filePath.filename().string(); fileName == "java")
            cmd.append("osintgram4j.jar");
        else
            cmd.append(fileName);
    }

    std::cout << "usage:" << std::endl;

    char userChar = '$';
#ifdef __linux__
    if (getuid() == 0 || geteuid() == 0)
        userChar = '#';
#elif defined(_WIN32)
    if (IsAdmin())
        userChar = '#';
#endif

    std::cout << userChar << " " << cmd << " [options] (target) (target2 ...)" << std::endl << std::endl;

    HelpPage hPage;
    hPage.setSpaceWidth(3);
    hPage.setStartSpaceWidth(4);
    hPage.setDescSeparator("=");
    hPage.addArg("-h  | --help", "", "Display usage and its help page");
    hPage.addArg("-D[key", "value]", "Puts setting values into the application runtime, temporarily overriding the App Settings file");
    hPage.addArg("-E[key", "value]", "Applies an environment variable to the Osintgram Shell");
    hPage.addArg("--test-exec-time", "", "Tests the execution time (does not start up the Shell");

#ifdef __linux__
    hPage.addArg("--sandbox", "PATH",
                 "Isolates the data saved by this tool from the entire disk (Linux exclusive, increases security factors)");
#endif

    std::string str = hPage.display();
    str = str.substr(0, str.length() - 1);

    std::cout << str << std::endl << std::endl;
    std::cout << "where:" << std::endl;
    std::cout << "   > [options]         Application Options" << std::endl;
    std::cout << "   > (target1)         Primary target; optional" << std::endl;
    std::cout << "   > (target2 ...)     Multiple optional targets" << std::endl;
}

#ifdef __linux__

void sigHandle(int) {
    if (AppShell::running)
        AppShell::stopShell(true);

    AppShell::cleanup();
}

#endif

void init() {
    // don't use loggers now, just initialize them (for now)
    Instances::getApplicationLoggingInstance();
    Instances::getNetworkLoggingInstance();
    Instances::getSecureLoggerInstance();

    if (Wine::WineExecution()) {
        Runtime::colorSupportEnabled = false;
    } else {
#ifdef _WIN32
        if (HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                if (!SetConsoleMode(hOut, dwMode))
                    Runtime::colorSupportEnabled = false;
            } else
                Runtime::colorSupportEnabled = false;
        } else
            Runtime::colorSupportEnabled = false;
#endif
    }

#ifdef __linux__
    signal(SIGINT, sigHandle);
    signal(SIGTERM, sigHandle);
    signal(SIGABRT, sigHandle);

    // presumably we ain't in a Terminal (presumable file piping)
    if (!isatty(STDOUT_FILENO))
        Runtime::colorSupportEnabled = false;
#endif

    // this method can still be called, will be ineffective, if called outside of Android platforms
    AndroidVer::prepare_cacerts();
}

void initSettings() {
}

void parseArgs(const std::vector<std::string> &args) {
    for (const std::string &arg: args) {
        std::string _arg = TrimString(arg);

        if (_arg.empty())
            continue;

        if (_arg[0] == '-') {
            if (_arg == "-h" || _arg == "--help") {
                usage();
                exit(0);
            }

            if (_arg.rfind("-E", 0) == 0) {
                std::string keyValue = arg.substr(2);
                size_t eqPos = keyValue.find('=');

                if (eqPos == std::string::npos) {
                    std::cerr << "Invalid format for -E argument (skipping): " << arg << std::endl;
                    continue;
                }

                AppShell::environment[keyValue.substr(0, eqPos)] = keyValue.substr(eqPos + 1);
            }

            if (_arg.rfind("-D", 0) == 0) {
                std::string keyValue = arg.substr(2);
                size_t eqPos = keyValue.find('=');

                if (eqPos == std::string::npos) {
                    std::cerr << "Invalid format for -D argument (skipping): " << arg << std::endl;
                    continue;
                }

                Runtime::defMap[keyValue.substr(0, eqPos)] = keyValue.substr(eqPos + 1);
            }

            if (_arg.rfind("--sandbox", 0) == 0) {
                size_t eqPos = arg.find('=');
                if (eqPos == std::string::npos) {
                    std::cerr << "Invalid format for --sandbox argument: " << arg << std::endl;
                    continue;
                }

                chrootPath = arg.substr(eqPos + 1);
            }
        }
    }
}

int main(int argc, char **argv) {
    std::set_terminate(exceptionHandler);
    init();

    // required: coloring system in "src/AppCommons/Terminal.cpp" under Windows systems
    WinSetColorMode();
    initSettings();

    if (argc > 1) {
        std::vector<std::string> args;
        args.reserve(argc - 1);

        for (int i = 1; i < argc; i++)
            args.emplace_back(argv[i]);

        parseArgs(args);
    }

    std::cout << TEXT_BLOCK() << std::endl << std::endl;

    // yes, I'm an asshole, but mainly because of certain warning messages going into the line of the Shell,
    // making things ugly in the process
    threadSleep(10);

    AppShell::initializeShell();
    ModLoader_load();

    // optional, by the CLI args, enable FS sandboxing
#ifdef __linux__
    if (!chrootPath.empty()) {
        if (!fs::exists(chrootPath)) {
            try {
                fs::create_directories(chrootPath);
            } catch (const fs::filesystem_error &err) {
                std::cerr << "Failed to create a directory at " << chrootPath << std::endl;
                std::cerr << "Error caused: " << err.what() << std::endl;
                return 1;
            }
        }

        if (fs::exists(chrootPath) && !fs::is_directory(chrootPath)) {
            std::cerr << "Unable to setup a secure environment (" << chrootPath << " is not a directory)" << std::endl;
            return 1;
        }

        bool shouldChroot = getuid() == 0;
#ifndef __ANDROID__
        if (!shouldChroot) {
            cap_t caps = cap_get_proc();
            if (!caps) {
                std::cerr << "Unable to fetch executable capabilities" << std::endl;
                return 1;
            }

            cap_flag_value_t cap_value;
            if (cap_get_flag(caps, CAP_SYS_CHROOT, CAP_EFFECTIVE, &cap_value) == -1) {
                std::cerr << "Failed to fetch chroot capabilities for the executable" << std::endl;
                cap_free(caps);
                return 1;
            }

            shouldChroot = cap_value == CAP_SET;
            cap_free(caps);
        }
#endif

        if (shouldChroot) {
            if (chroot(chrootPath.c_str()) != 0) {
                std::cerr << "Sandboxing failed, error: " << std::strerror(errno) << std::endl;
                return 1;
            }

            if (chdir("/") != 0) {
                std::cerr << "Root Change failed, error: " << std::strerror(errno) << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Sandboxing failed: not root";

#ifdef __ANDROID__
            std::cerr << ", which requires the process to enable Sandboxing capability" << std::endl;
#else
            std::cerr << ", nor given the capability to do so" << std::endl;
#endif

            return 1;
        }
    }
#endif

    ModLoader_start();

    threadSleep(10);

    AppShell::launchShell();

    return 0;
}