#ifndef OSINTGRAMCXX_TERMINAL_HPP
#define OSINTGRAMCXX_TERMINAL_HPP

#include <iostream>
#include <string>
#include <sstream>

class Terminal {
public:
    enum class TermColor {
        RESET,
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        PURPLE,
        CYAN,
        WHITE
    };

    static void print(TermColor color, const std::string& msg, bool reset = true);
    static void print(TermColor color, const std::stringstream & msg, bool reset = true);
    static void println(TermColor color, const std::string& msg, bool reset = true);
    static void println(TermColor color, const std::stringstream & msg, bool reset = true);
    static void errPrint(TermColor color, const std::string& msg, bool reset = true);
    static void errPrint(TermColor color, const std::stringstream & msg, bool reset = true);
    static void errPrintln(TermColor color, const std::string& msg, bool reset = true);
    static void errPrintln(TermColor color, const std::stringstream & msg, bool reset = true);
    static void clearTerminal();

    static int windowColumns();
    static int windowRows();

private:
    static std::string translateColor(TermColor color);
};

#endif //OSINTGRAMCXX_TERMINAL_HPP
