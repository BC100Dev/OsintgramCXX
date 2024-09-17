#ifndef OSINTGRAMCXX_UTILS_HPP
#define OSINTGRAMCXX_UTILS_HPP

#include <map>
#include <string>
#include <sstream>

namespace OsintgramCXX {

    std::string TrimString(const std::string &str) {
        size_t start = str.find_first_not_of(" \t\n");
        size_t end = str.find_last_not_of(" \t\n");

        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    std::map<int, std::string> SplitString(const std::string& str, const std::string& delim) {
        std::map<int, std::string> tokens;
        size_t startIndex = 0;
        size_t delimPos = 0;
        int tokenIndex = 0;

        while ((delimPos = str.find(delim, startIndex)) != std::string::npos) {
            tokens[tokenIndex++] = str.substr(startIndex, delimPos - startIndex);
            startIndex = delimPos + delim.length();
        }

        tokens[tokenIndex] = str.substr(startIndex);

        return tokens;
    }

    bool StartsWith(const std::string& str, const std::string& prefix) {
        if (prefix.size() > str.size())
            return false;

        return std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    bool EndsWith(const std::string& str, const std::string& suffix) {
        if (suffix.size() > str.size())
            return false;

        return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

}

#endif //OSINTGRAMCXX_UTILS_HPP
