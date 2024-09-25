#include <osintgram/shell/Shell.hpp>

#include <AppCommons/Utils.hpp>
#include <AppCommons/Terminal.hpp>

#include <sstream>
#include <thread>
#include <iostream>

namespace OsintgramCXX::Shell {

    std::string PS1;
    bool running = false;
    bool shellInitialized = false;
    ShellEnvironment environment;
    std::vector<ShellCommand> shellCommands{};

    void initializeShell() {
        std::string user = CurrentUsername();
#ifdef _WIN32
        std::map<int, std::string> _user_splits = OsintgramCXX::SplitString(user, "\\");
        user = _user_splits[1];
#endif

        std::stringstream strStream;
        strStream << "[" << user << "/" << GetHostname() << ": " << CurrentWorkingDirectory() << "]"
                  << (IsAdmin() ? "$" : "#") << " ";

        PS1 = strStream.str();

        shellInitialized = true;
    }

    void modEnviron(const std::string& line) {
        std::string worker = TrimString(line);
        if (worker[0] == '&')
            worker = worker.substr(1);

        if (StringContains(line, "=")) {
            std::map<int, std::string> opt = SplitString(worker, "=", 2);
            opt[0] = TrimString(opt[0]);
            opt[1] = TrimString(opt[1]);

            if (opt[0] == "PS1") {
                PS1 = opt[1];
                return;
            }

            environment[opt[0]] = opt[1];
        } else {
            auto it = environment.find(worker);
            if (it == environment.end()) {
                Terminal::errPrintln(Terminal::TermColor::RED, worker + " (not found)", true);
                return;
            }

            Terminal::print(Terminal::TermColor::BLUE, worker, true);
            std::cout << " => ";
            Terminal::print(Terminal::TermColor::CYAN, it->second, true);
        }
    }

    void cmd() {
        std::string line;

        while (running) {
            std::cout << PS1;
            std::getline(std::cin, line);

            line = TrimString(line);

            if (line[0] == '&')
                modEnviron(line);

            if (line == "exit" || line == "quit") {
                stopShell();
                return;
            }
        }
    }

    void launchShell() {
        running = true;
        std::string line;

        try {
            std::thread shellThread(cmd);
            shellThread.join();
        } catch (std::exception& ex) {
            std::cerr << "Shell Thread Error (Shell.cpp): " << ex.what() << std::endl;
            stopShell();
        }
    }

    void stopShell() {
        running = false;
    }

}