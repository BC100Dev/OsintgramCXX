#include "FuncConstructor.hpp"

std::pair<std::string, std::string> SplitKV(const std::string& str) {
    size_t pos = str.find('=');
    if (pos == std::string::npos)
        return {str, ""};

    return {
        str.substr(0, pos),
        str.substr(pos + 1)
    };
}

FuncParams MakeFuncParams(int argc, char **argv, int envc, char **env_map) {
    std::vector<std::string> args;
    args.reserve(argc);
    for (int i = 0; i < argc; i++)
        args.emplace_back(argv[i]);

    std::map<std::string, std::string> env;
    for (int i = 0; i < envc; i++) {
        auto [key, val] = SplitKV(env_map[i]);
        env[key] = val;
    }

    return FuncParams{args, env};
}