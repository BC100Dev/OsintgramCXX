#ifndef OSINTGRAMCXX_FUNCHEADERS_HPP
#define OSINTGRAMCXX_FUNCHEADERS_HPP

#include <functional>
#include <string>
#include <map>
#include <vector>

namespace CoreFunctions {

    // Parameters:
    // 1. string - command name itself
    // 2. function(vec<string>, map<string, string>) - command params
    extern std::map<std::string, std::function<int(const std::vector<std::string>, const std::map<std::string, std::string>)>> funcMap;

}

// Functions for each method

int cache_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env);

int dsi_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env);

int net_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env);

int session_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env);

int user_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env);

#endif //OSINTGRAMCXX_FUNCHEADERS_HPP
