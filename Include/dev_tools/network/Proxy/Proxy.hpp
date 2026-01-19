#ifndef OSINTGRAMCXX_PROXY_HPP
#define OSINTGRAMCXX_PROXY_HPP

#include <string>
#include <vector>
#include <dev_tools/network/Network.hpp>

namespace DevUtils {

    enum ProxyConnectorType {
        HTTP = 0,
        HTTPS = 1,
        SOCKS4 = 2,
        SOCKS5 = 3
    };

    struct ProxyResponse {
        bool connected;
        int statusCode;
        std::vector<char> proxyResponseData;
        ResponseData* endpointResponseData;
    };

    struct ProxyInfo {
        std::string host;
        int port;
        std::string* username;
        std::string* password;
        ProxyConnectorType conType;
    };

    ProxyResponse CreateProxyRequest(const ProxyInfo& proxy, const RequestData& requestData);

}

#endif //OSINTGRAMCXX_PROXY_HPP
