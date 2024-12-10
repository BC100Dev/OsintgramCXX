#ifndef OSINTGRAMCXX_COOKIES_HPP
#define OSINTGRAMCXX_COOKIES_HPP

#include <map>
#include <string>
#include <any>
#include <vector>
#include <optional>

namespace OsintgramCXX::Authentication {

    enum Cookie_SameSiteType {
        Strict,
        Lax,
        None
    };

    class CookieInfo {
    public:
        struct Fields {
            std::optional<std::string> domain;
            std::optional<std::string> expiryDate;
            std::optional<bool> httpOnly;
            std::optional<int> maxAge;
            std::optional<bool> partitioned;
            std::optional<std::string> path;
            std::optional<bool> secure;
            std::optional<Cookie_SameSiteType> sameSite;
        };

        std::string cookieName;
        std::string cookieValue;

        ~CookieInfo() = default;

        static CookieInfo ParseSetCookieHeader(const std::string &headerValue);

        CookieInfo(std::string name, std::string value, Fields fields);

    private:
        Fields fields;
    };

    std::string WriteToHeader(bool rawTcpHeader, const std::vector<CookieInfo>& cookies);

}

#endif //OSINTGRAMCXX_COOKIES_HPP
