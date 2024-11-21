#ifndef OSINTGRAMCXX_PROXY_HPP
#define OSINTGRAMCXX_PROXY_HPP

#include <string>

namespace AppNetworking::Proxy {

    enum ProxyConnectorType {
        HTTP = 0,
        HTTPS = 1,
        SOCKS4 = 2,
        SOCKS5 = 3
    };

    struct Proxy {
        std::string host;
        int port;
        std::string username;
        std::string password;
        ProxyConnectorType conType;
    };

}

#endif //OSINTGRAMCXX_PROXY_HPP
