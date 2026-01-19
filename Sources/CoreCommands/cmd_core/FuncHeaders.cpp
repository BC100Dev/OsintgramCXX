// Required for the funcMap
#include "FuncHeaders.hpp"

namespace CoreFunctions {

    std::map<std::string, std::function<int(const std::vector<std::string>&, const std::map<std::string, std::string>&)>> funcMap = {
        {"dsi", dsi_func},
        {"env", env_func},
        {"userctl", user_func},
        {"cachectl", cache_func},
        {"sessionctl", session_func},
        {"net", net_func},
    };

}