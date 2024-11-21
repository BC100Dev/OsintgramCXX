#ifndef OSINTGRAMCXX_NETWORKING_HPP
#define OSINTGRAMCXX_NETWORKING_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <curl/curl.h>

namespace OsintgramCXX::Networking {

    struct ResponseData {

        bool isBinary;
        std::string stringData;
        std::vector<unsigned char> binaryData;
        std::stringstream error;

    };

    using HeaderMap = std::map<std::string, std::string>;

    ResponseData GetRequest(const std::string& url, const HeaderMap& headers);

    ResponseData PostRequestS(const std::string& url, const HeaderMap& headers, const std::string& data);
    ResponseData PostRequestB(const std::string& url, const HeaderMap& headers, const std::vector<unsigned char>& data);

    ResponseData DeleteRequest(const std::string& url, const HeaderMap& headers);

}

#endif //OSINTGRAMCXX_NETWORKING_HPP
