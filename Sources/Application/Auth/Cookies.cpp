#include <OsintgramCXX/App/Auth/Cookies.hpp>

#include <sstream>
#include <utility>

#include <OsintgramCXX/Commons/Utils.hpp>

namespace OsintgramCXX::Authentication {

    CookieInfo::CookieInfo(std::string name, std::string value,
                           Fields fields) :
                           cookieName(std::move(name)), cookieValue(std::move(value)), fields(std::move(fields)) {}

    CookieInfo CookieInfo::ParseSetCookieHeader(const std::string& headerValue) {
        std::istringstream stream(headerValue);
        std::string segment;
        std::string name;
        std::string value;
        Fields parsedFields;

        bool isFirstSegment = true;

        while (std::getline(stream, segment, ';')) {
            auto pos = segment.find('=');
            std::string key = pos != std::string::npos ? segment.substr(0, pos) : segment;
            std::string val = pos != std::string::npos ? segment.substr(pos + 1) : "";

            key = TrimString(key);
            val = TrimString(val);

            if (isFirstSegment) {
                isFirstSegment = false;
                name = key;
                value = val;
            } else if (key == "Domain")
                parsedFields.domain = val;
            else if (key == "Path")
                parsedFields.path = val;
            else if (key == "Max-Age")
                parsedFields.maxAge = std::stoi(val);
            else if (key == "HttpOnly")
                parsedFields.httpOnly = true;
            else if (key == "SameSite") {
                if (val == "Strict")
                    parsedFields.sameSite = Cookie_SameSiteType::Strict;
                else if (val == "Lax")
                    parsedFields.sameSite = Cookie_SameSiteType::Lax;
                else if (val == "None")
                    parsedFields.sameSite = Cookie_SameSiteType::None;
            } else if (key == "Partitioned")
                parsedFields.partitioned = true;
            else if (key == "Expires")
                parsedFields.expiryDate = val;
            else if (key == "Secure")
                parsedFields.secure = true;
        }

        return {name, value, parsedFields};
    }

    std::string WriteToHeader(bool rawTcpHeader, const std::vector<CookieInfo>& cookies) {
        std::ostringstream hv;
        if (rawTcpHeader)
            hv << "Cookie: ";

        for (size_t i = 0; i < cookies.size(); i++) {
            const auto& cookie = cookies[i];
            hv << cookie.cookieName << "=" << cookie.cookieValue;

            if (i < cookies.size() - 1)
                hv << "; ";
        }

        if (rawTcpHeader)
            hv << "\r\n";

        return hv.str();
    }

}