#include <iostream>
#include <exception>
#include <vector>
#include <stdexcept>
#include <thread>
#include <csignal>
#include <sstream>
#include <typeinfo>
#include <climits>
#include <filesystem>
#include <set>

#include <osintgram/shell/ShellEnv.hpp>
#include <osintgram/Defaults.hpp>
#include <osintgram/Properties.hpp>
#include <osintgram/AppProps.hpp>

#include <AppCommons/HelpPage.hpp>
#include <AppCommons/Terminal.hpp>
#include <AppCommons/Utils.hpp>

#ifdef _WIN32
#include <windows.h>
#else

#include <unistd.h>

#endif

namespace fs = std::filesystem;

std::set<std::string> processedCmdParamFiles;

void parseFileArgs(const std::string &filePath);

void exceptionHandler() {
    std::exception_ptr exptr = std::current_exception();

    if (exptr) {
        try {
            std::rethrow_exception(exptr);
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
    hPage.addArg("-E[key=value]", "", "Applies an environment variable to the Osintgram Shell");
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

void init() {
    //OsintgramCXX::InitLoadSettings();
}

void testThread() {
    std::cout << "From test thread" << std::endl;
}

void appParseArgs(const std::vector<std::string>& args, const std::string& fileCall) {
    for (const std::string& arg : args) {
        if (arg[0] == '-') {
            if (arg == "-h" || arg == "--help") {
                usage();
                exit(0);
            }

            if (arg == "-Sw" || arg == "--suppress-warnings") {
                std::cerr << "Suppressing all warnings" << std::endl;
                std::cerr << "Only apply this warning, when you know, what you are doing." << std::endl;

                OsintgramCXX::suppressWarnings = true;
            }

            if (arg.rfind("-E", 0) == 0 && arg.length() > 2) {
                std::string keyValue = arg.substr(2);
                size_t eqPos = keyValue.find('=');

                if (eqPos == std::string::npos) {
                    std::cerr << "Invalid format for -E argument (skipping): " << arg << std::endl;
                    continue;
                }

                std::string key = keyValue.substr(0, eqPos);
                std::string val = keyValue.substr(eqPos + 1);

                OsintgramCXX::ShellEnvironment envMap;
                envMap.name = key;
                envMap.value = val;

                OsintgramCXX::shellEnvMap.push_back(envMap);
            }
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

            OsintgramCXX::targetList.push_back(target);
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

    // required: coloring system in "src/AppCommons/Terminal.cpp"
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    init();

    if (argc > 1)
        initParseArgs(argc, argv);

    std::cout << TEXT_BLOCK() << std::endl;
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
        std::string processLine = OsintgramCXX::TrimString(line);
        if (processLine.empty())
            continue;

        if (processLine[0] == '#')
            continue;

        args.push_back(line);
    }

    appParseArgs(args, filePath);
}