#include <OsintgramCXX/App/Auth/TwoFactor.hpp>

namespace OsintgramCXX::Authentication {

    bool Require2FA(const json &authData) {
        return false;
    }

    TwoWayMethod FetchAuthorizationMethod(const json &authData) {
        return {};
    }

    // this method passes onto several arguments:
    // - code: the 2FA code
    // - method: The method that was used to enter the 2FA code (SMS / APP)
    // - initialResponseData: The first initial request that was made for authentication (/accounts/login/ajax)
    // - sendCodeData: (presumably needed) request information that was used to send the code (if SMS)
    // - userCookies: The necessary authentication cookies
    // - requestHeaders: The necessary request headers that holds the information for further requests
    TwoFA_Validation Validate2FA(const std::string &code, const TwoWayMethod &method,
                                 const ResponseData &initialResponseData, const ResponseData *sendCodeData,
                                 std::vector<CookieInfo> &userCookies, const Headers &requestHeaders) {
        return {};
    }

}