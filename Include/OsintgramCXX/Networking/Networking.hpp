#ifndef OSINTGRAMCXX_NETWORKING_HPP
#define OSINTGRAMCXX_NETWORKING_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <optional>
#include <curl/curl.h>
#include <iostream>

namespace OsintgramCXX::Networking {

    using Headers = std::vector<std::pair<const std::string, const std::string>>;
    using ByteData = std::vector<char>;
    using RawData = std::vector<unsigned char>;

    enum RequestMethod {
        REQ_GET = 0,
        REQ_POST = 1,
        REQ_PATCH = 2,
        REQ_HEAD = 3,
        REQ_PUT = 4,
        REQ_DELETE = 5,
        REQ_CONNECT = 6,
        REQ_OPTIONS = 7,
        REQ_TRACE = 8
    };

    enum HttpVersion {
        HTTP_1_0 = 1,
        HTTP_1_1 = 2,
        HTTP_2_0 = 3,
        HTTP_3_0 = 4,
    };

    struct ResponseData {
        int statusCode = 0;
        ByteData data;
        RawData raw;
        Headers headers;
        HttpVersion version;
        std::string errorData;
    };

    struct RequestData {
        RequestMethod method = REQ_GET;
        std::string url;
        Headers headers;
        HttpVersion version = HTTP_1_1;
        ByteData postData;
        RawData postRaw;
        std::string ca_path;

        long connTimeoutMillis = 30000;
        long readTimeoutMillis = 15000;

        bool followRedirects = true;
        bool verifySSL = true;
    };

    ResponseData CreateRequest(const RequestData &request);

    class NetworkError : public std::runtime_error {
    public:
        explicit NetworkError(const std::string& what) : std::runtime_error(what) {}
    };

}

#endif //OSINTGRAMCXX_NETWORKING_HPP
