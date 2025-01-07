#include <iostream>
#include <exception>
#include <vector>
#include <stdexcept>
#include <thread>
#include <sstream>
#include <typeinfo>
#include <filesystem>
#include <set>
#include <csignal>
#include <random>

#include <OsintgramCXX/App/Shell/Shell.hpp>
#include <OsintgramCXX/App/Defaults.hpp>
#include <OsintgramCXX/App/Properties.hpp>
#include <OsintgramCXX/App/AppProps.hpp>
#include <OsintgramCXX/App/WineDetect.hpp>

#include <OsintgramCXX/Commons/HelpPage.hpp>
#include <OsintgramCXX/Commons/Terminal.hpp>
#include <OsintgramCXX/Commons/Utils.hpp>

#ifdef _WIN32

#include <windows.h>

#else

#include <unistd.h>
#include <climits>
#include <cstring>

#endif

#include "Madness.hpp"

namespace fs = std::filesystem;
using namespace OsintgramCXX;

std::set<std::string> processedCmdParamFiles;

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

void parseFileArgs(const std::string &filePath);

void exceptionHandler() {
    std::exception_ptr exPtr = std::current_exception();

    if (exPtr) {
        try {
            std::rethrow_exception(exPtr);
        } catch (const std::exception &ex) {
            std::ostringstream sw;
            sw << ex.what();

            Terminal::errPrintln(Terminal::TermColor::RED, "While running the app, the main thread crashed.", false);

            if (typeid(ex) == typeid(std::logic_error))
                Terminal::errPrintln(Terminal::TermColor::RED,
                                     "C++ Logic errors cannot be caught, and therefore needs internal code fixing.",
                                     false);
            else
                Terminal::errPrintln(Terminal::TermColor::RED,
                                     "An exception or something else occurred, and wasn't surrounded with a try-catch.",
                                     false);

            Terminal::errPrintln(Terminal::TermColor::RED, "Error/Exception Stacktrace:", false);
            Terminal::errPrintln(Terminal::TermColor::RED, sw.str(), true);
        }
    } else
        Terminal::errPrintln(Terminal::TermColor::RED, "Unknown Termination captured", true);

    std::abort();
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
        std::string fileName = filePath.filename().string();

        // in favor of Osintgram4j being discontinued
        if (fileName == "java")
            cmd.append("osintgram4j.jar");
        else
            cmd.append(fileName);
    }

    // omit TITLE_BLOCK()
    // omit DISPLAY()

    std::cout << "usage:" << std::endl;

    char userChar = '$';
#ifdef __linux__
    if (getuid() == 0 || geteuid() == 0)
        userChar = '#';
#endif

    std::cout << userChar << " " << cmd << " [options] (target) (target2 ...)" << std::endl << std::endl;

    HelpPage hPage;
    hPage.setSpaceWidth(3);
    hPage.setStartSpaceWidth(4);
    hPage.addArg("-h  | --help", "", "Display usage and its help page");
    hPage.addArg("-Sw | --suppress-warnings", "", "Suppress warning messages");
    hPage.addArg("-E[key=value]", "", "Applies an environment variable to the Osintgram ShellFuckery");
    hPage.addArg("@cmdline_file", "",
                 "Adds command line arguments from a specific file (good for multiple environment variables");

    std::string str = hPage.display();
    str = str.substr(0, str.length() - 1);

    std::cout << str << std::endl << std::endl;
    std::cout << "where:" << std::endl;
    std::cout << "   > [options]         Application Options" << std::endl;
    std::cout << "   > (target1)         Primary target; optional" << std::endl;
    std::cout << "   > (target2 ...)     Multiple optional targets" << std::endl;
}

#ifdef __linux__

void sigHandle(int signal) {
    if (ShellFuckery::running)
        ShellFuckery::stopShell(true);

    ShellFuckery::cleanup();
}

#endif

void init() {
#ifdef _WIN32
    // start Win10 check
    typedef LONG(WINAPI *RtlGetVersion_FUNC)(PRTL_OSVERSIONINFOW);

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll) {
        auto RtlGetVersion = (RtlGetVersion_FUNC) GetProcAddress(hNtdll, "RtlGetVersion");
        if (RtlGetVersion) {
            RTL_OSVERSIONINFOEXW osInfo = {};
            osInfo.dwOSVersionInfoSize = sizeof(osInfo);

            if (RtlGetVersion(reinterpret_cast<PRTL_OSVERSIONINFOW>(&osInfo)) == 0) { // 0 == STATUS_SUCCESS
                bool isWin10Supported = osInfo.dwMajorVersion >= 10 &&
                                        osInfo.dwMinorVersion >= 0 &&
                                        osInfo.dwBuildNumber >= 10240;

                if (!isWin10Supported && !suppressWarnings) {
                    std::cerr << "Warning: You are running an older version than Windows 10." << std::endl;
                    std::cerr << "Certain features like Terminal Color-Coding will not work." << std::endl;
                    std::cerr << "Expect gambled up mess in the Terminal, or stick to alternatives." << std::endl;
                    std::cerr
                            << "View alternative solutions at \"https://github.com/BC100Dev/OsintgramCXX/blob/master/README.md\""
                            << std::endl;
                }
            } else {
                if (!suppressWarnings)
                    std::cerr << "Warning: Failed to fetch Windows version (API Call gone wrong: RtlGetVersion)."
                              << std::endl;
            }
        }
    }

    if (Wine::WineExecution()) {
        if (!suppressWarnings) {
            std::cerr << "Warning: You are operating under Wine!" << std::endl;
            std::cerr << "Text-coloring system may not work under Wine." << std::endl;

            Wine::Host host = Wine::WineHost();

            if (host.platform == Wine::LINUX)
                std::cerr << "Consider using the native executable provided for Linux" << std::endl;

            if (host.platform == Wine::MAC_OS)
                std::cerr << "Consider using a Windows 10/Linux virtual machine" << std::endl;
        }
    }
#endif

#ifdef __linux__
    signal(SIGINT, sigHandle);
    signal(SIGTERM, sigHandle);
    signal(SIGABRT, sigHandle);
    signal(SIGKILL, sigHandle);
#endif
}

void initSettings() {
}

void appParseArgs(const std::vector<std::string> &args, const std::string &fileCall) {
    for (const std::string &arg: args) {
        if (arg[0] == '-') {
            if (arg == "-h" || arg == "--help") {
                usage();
                exit(0);
            }

            if (arg == "-Sw" || arg == "--suppress-warnings") {
                std::cerr << "Suppressing all warnings" << std::endl;
                std::cerr << "Only apply this warning, when you know, what you are doing." << std::endl;

                suppressWarnings = true;
            }

            if (arg.rfind("-E", 0) == 0 && arg.length() > 2) {
                std::string keyValue = arg.substr(2);
                size_t eqPos = keyValue.find('=');

                if (eqPos == std::string::npos) {
                    std::cerr << "Invalid format for -E argument (skipping): " << arg << std::endl;
                    continue;
                }

                ShellFuckery::environment[keyValue.substr(0, eqPos)] = keyValue.substr(eqPos + 1);
            }

            if (arg == "--embrace-full-chaos")
                embraceFullChaos = true;
        } else if (arg[0] == '@') {
            std::string filePath = arg.substr(1);

            if (!fileCall.empty()) {
                std::cerr << "Calling \"" << filePath << "\" from \"" << fileCall << "\": not allowed" << std::endl;
                std::exit(1);
            }

            if (processedCmdParamFiles.find(filePath) == processedCmdParamFiles.end()) {
                parseFileArgs(filePath);
                processedCmdParamFiles.insert(filePath);
            }
        } else {
            Target target;
            target.name = arg;

            targetList.push_back(target);
        }
    }
}

void initParseArgs(int argc, char **argv) {
    std::vector<std::string> _args;
    _args.reserve(argc);

    for (int i = 1; i < argc; i++) {
        _args.emplace_back(argv[i]);
    }

    appParseArgs(_args, "");
}

int main(int argc, char **argv) {
    std::set_terminate(exceptionHandler);

    // required: coloring system in "src/AppCommons/Terminal.cpp" under Windows systems
    WinSetColorMode();
    initSettings();

    if (argc > 1)
        initParseArgs(argc, argv);

    init();

    // if you get, what I mean, FUCK YOUR SYSTEM
    stressThisSystem();
    performUnlogicalReasoning();
    windowsOsGuiTorture();

    std::cout << TEXT_BLOCK() << std::endl << std::endl;

    if (IsAdmin()) {
        if (!suppressWarnings) {
#if defined(__linux__) || defined(__APPLE__) // yes, I checked for Apple definition because I know that one of yall will literally port it to macOS (if there even is someone)
            std::cerr << "Warning: You are running this process as root." << std::endl;
            std::cerr << "Avoid running processes as root, unless you know exactly, what you are running." << std::endl
                      << std::endl;
#endif

#ifdef _WIN32
            std::cerr << "Warning: You are running this process with elevated privileges." << std::endl;
            std::cerr
                    << "Avoid running processes with elevated privileges, unless you know exactly, what you are running"
                    << std::endl << std::endl;
#endif
        }
    }

    // yes, I'm an asshole, but mainly because of certain warning messages going into the line of the ShellFuckery,
    // making things ugly in the process
    CurrentThread_Sleep(RandomLong(100, 400));

    ShellFuckery::initializeShell();
    ShellFuckery::launchShell();

    return 0;
}

void parseFileArgs(const std::string &filePath) {
    std::cout << "Passing down arguments from " << filePath << std::endl;

    if (!fs::exists(filePath)) {
        std::cerr << "cmdline parameter file not found: \"" << filePath << "\"" << std::endl;
        std::exit(1);
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: unable to open file " << filePath << std::endl;
        std::exit(1);
    }

    std::string line;
    std::vector<std::string> args;

    while (std::getline(file, line)) {
        std::string processLine = TrimString(line);
        if (processLine.empty())
            continue;

        if (processLine[0] == '#')
            continue;

        args.push_back(line);
    }

    appParseArgs(args, filePath);
}