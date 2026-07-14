#ifndef INSTAGRAM_HIKERAPI_HPP
#define INSTAGRAM_HIKERAPI_HPP

#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <dev_tools/network/Network.hpp>

#include <IGApi/Session.hpp>

#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace IG::Session {
    static std::string HIKER_API_URL_PREFIX = "https://api.hikerapi.com";
    static std::string HIKER_API_IG_PREFIX = HIKER_API_URL_PREFIX + "/v2";

    struct HikerBalance {
        int requests;
        int rate;
        double amount;
        std::string currency;
    };

    struct HikerBody {
        std::string contentType;
        std::variant<std::string, std::vector<unsigned char>> body;
    };

    struct HikerRequest {
        std::string endpoint;
        std::string accepts;
        RequestMethod method;
        Headers extraHeaders;
        std::optional<HikerBody> body;
    };

    struct HikerResponse {
        int statusCode;
        Headers headers;
        HikerBody body;
    };

    class HikerAPI {
    public:
        explicit HikerAPI(const std::string& authToken);

        HikerBalance GetBalance();

        HikerResponse InitiateHikerRequest(const HikerRequest& request);

    private:
        std::string m_authToken;

        bool validateToken();
    };

    class HikerError : public std::runtime_error {
    public:
        explicit HikerError(const std::string& err) : runtime_error(err) {
        }

        explicit HikerError(const std::string& err, std::string& data) : runtime_error(err + " (data: \"" + data + "\")") {
        }
    };

    std::string GetStringBodyVariant();

    HikerAPI& GetSharedHikerInstance();

    HikerAPI InitializeSharedHikerInstance(const std::string& authToken);

    HikerAPI InitializeSharedHikerInstance(const std::string& authToken, bool override);
}

#endif //INSTAGRAM_HIKERAPI_HPP
