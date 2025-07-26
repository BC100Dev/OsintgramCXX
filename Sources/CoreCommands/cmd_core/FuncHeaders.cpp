// Required for the funcMap
#include "FuncHeaders.hpp"

namespace CoreFunctions {

    std::map<std::string, std::function<int(const std::vector<std::string>, const std::map<std::string, std::string>)>> funcMap;

}