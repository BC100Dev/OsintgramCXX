#include "FuncHeaders.hpp"

#include <iostream>

int user_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env) {
    std::cout << "Calling out of the user function method" << std::endl;
    return 0;
}