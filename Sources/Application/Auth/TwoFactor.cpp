#include <OsintgramCXX/App/Auth/TwoFactor.hpp>

namespace OsintgramCXX::Authentication {

    bool Require2FA(const json &authData) {
        return false;
    }

    TwoWayMethod FetchAuthorizationMethod(const json &authData) {
        return {};
    }

    TwoFA_Validation Validate2FA(const std::string &code, const TwoWayMethod &method,
                                 const ResponseData &initialResponseData, const ResponseData *sendCodeData,
                                 std::vector<CookieInfo> &userCookies, const Headers &requestHeaders) {
        return {};
    }

}