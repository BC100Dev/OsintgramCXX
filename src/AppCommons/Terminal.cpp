#include <AppCommons/Terminal.hpp>

#ifdef __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

void Terminal::print(TermColor color, const std::string &msg, bool reset) {
    if (!msg.empty())
        std::cout << translateColor(color) << msg;

    if (reset)
        std::cout << translateColor(TermColor::RESET);
}

void Terminal::println(TermColor color, const std::string &msg, bool reset) {
    Terminal::print(color, msg, reset);
    std::cout << std::endl;
}

void Terminal::errPrint(TermColor color, const std::string &msg, bool reset) {
    if (!msg.empty())
        std::cerr << translateColor(color) << msg;

    if (reset)
        std::cerr << translateColor(TermColor::RESET);
}

void Terminal::errPrintln(TermColor color, const std::string &msg, bool reset) {
    Terminal::print(color, msg, reset);
    std::cerr << std::endl;
}

std::string Terminal::translateColor(TermColor color) {
    switch (color) {
        case TermColor::RESET:   return "\x1B[0m";
        case TermColor::BLACK:   return "\x1B[0;30m";
        case TermColor::RED:     return "\x1B[0;31m";
        case TermColor::GREEN:   return "\x1B[0;32m";
        case TermColor::YELLOW:  return "\x1B[0;33m";
        case TermColor::BLUE:    return "\x1B[0;34m";
        case TermColor::PURPLE:  return "\x1B[0;35m";
        case TermColor::CYAN:    return "\x1B[0;36m";
        case TermColor::WHITE:   return "\x1B[0;37m";
        default:                 return "\x1B[0m";
    }
}

void Terminal::clearTerminal() {
    std::cout << "\033[H\033[2J";
    std::cout.flush();
}

int Terminal::windowColumns() {
#ifdef __linux__
    struct winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        return -1;

    return w.ws_col;
#endif

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;

    return -1;
#endif
}

int Terminal::windowRows() {
#ifdef __linux__
    struct winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        return -1;

    return w.ws_row;
#endif

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    return -1;
#endif
}