#ifndef OSINTGRAMCXX_NETWORKING_HPP
#define OSINTGRAMCXX_NETWORKING_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <optional>
#include <curl/curl.h>

namespace OsintgramCXX::Networking {

    using Headers = std::map<std::string, std::string>;
    using ByteData = std::vector<char>;

    enum RequestMethod {
        GET = 0,
        POST = 1,
        PATCH = 2,
        HEAD = 3,
        PUT = 4,

        // We shisha deleted the Windows macro conflict because as usual, compiling for Windows
        // must be an ass
        SHISHA_DELETE = 5,
        CONNECT = 6,
        OPTIONS = 7,
        TRACE = 8
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
        Headers headers;
        HttpVersion version;
        std::string errorData;
    };

    struct RequestData {
        RequestMethod method;
        std::string url;
        Headers headers;
        HttpVersion version;
        ByteData postData;

        long connTimeoutMillis = 30000;
        long readTimeoutMillis = 15000;

        bool followRedirects = true;
        bool verifySSL = true;
    };

    ResponseData CreateRequest(const RequestData &request);

}

#endif //OSINTGRAMCXX_NETWORKING_HPP
