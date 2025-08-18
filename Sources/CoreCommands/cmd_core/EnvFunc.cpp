#include "FuncHeaders.hpp"
#include <iostream>

int env_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env) {
    if (env.empty()) {
        std::cerr << "environment map is empty" << std::endl;
        return 1;
    }

    for (const auto& it: env) {
        std::cout << it.first << " => " << it.second << std::endl;
    }

    return 0;
}