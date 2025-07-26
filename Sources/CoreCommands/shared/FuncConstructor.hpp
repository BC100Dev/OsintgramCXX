#ifndef OSINTGRAMCXX_FUNCCONSTRUCTOR_HPP
#define OSINTGRAMCXX_FUNCCONSTRUCTOR_HPP

#include <string>
#include <vector>
#include <map>

struct FuncParams {

    std::vector<std::string> args;
    std::map<std::string, std::string> env;

};

FuncParams MakeFuncParams(int argc, char** argv, int envc, char** env_map);

#endif //OSINTGRAMCXX_FUNCCONSTRUCTOR_HPP
