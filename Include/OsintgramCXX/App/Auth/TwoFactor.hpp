#ifndef OSINTGRAMCXX_TWOFACTOR_HPP
#define OSINTGRAMCXX_TWOFACTOR_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "OsintgramCXX/Networking/Networking.hpp"

#include "../../../../Sources/Application/Auth/Cookies.hpp"

using json = nlohmann::json;
using namespace OsintgramCXX::Networking;

// Blueprint for 2FA Authentication
// This is a blueprint concept for the 2FA authentication. This will tell, if
// - We have SMS 2FA, which is highly not recommended
// - We have TOTP, which is highly recommended (e.g. Google Authenticator)
// and, of course, whether we can make the final Authorization Request

namespace OsintgramCXX::Authentication {

    enum TwoWayMethod {
        SMS = 0,
        APP = 1,

        // This is WhatsApp or other
        SMS_SECURE = 2
    };

    enum TwoFA_ReturnType {
        SUCCESS = 0,
        INVALID_CODE = 1,
        TIMEOUT = 2,
        NETWORK_ERROR = 3,
        UNAUTHORIZED = 4,
        OTHER = 5
    };

    struct TwoFA_Validation {
        TwoFA_ReturnType retType = OTHER;
        ResponseData responseData{};
    };

    bool Require2FA(const json &authData);

    TwoWayMethod FetchAuthorizationMethod(const json &authData);

    // this method passes onto several arguments:
    // - code: the 2FA code
    // - method: The method that was used to enter the 2FA code (SMS / APP)
    // - initialResponseData: The first initial request that was made for authentication (/accounts/login/ajax)
    // - sendCodeData: (presumably needed) request information that was used to send the code (if SMS (Secure) is used)
    // - userCookies: The necessary authentication cookies
    // - requestHeaders: The necessary request headers that holds the information for further requests
    TwoFA_Validation Validate2FA(const std::string &code, const TwoWayMethod &method,
                                 const ResponseData &initialResponseData, const ResponseData *sendCodeData,
                                 const std::vector<CookieInfo> &cookies, const Headers &requestHeaders);

}

#endif //OSINTGRAMCXX_TWOFACTOR_HPP
