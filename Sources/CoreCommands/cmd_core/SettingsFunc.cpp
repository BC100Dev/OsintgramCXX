#include "FuncHeaders.hpp"

#include <iostream>

int set_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env) {
    std::cout << "Calling out of the settings function method" << std::endl;
    return 0;
}