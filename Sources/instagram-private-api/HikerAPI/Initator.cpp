#include <IGApi/HikerAPI.hpp>

#include <dev_tools/network/Network.hpp>

#include <dev_tools/commons/Utils.hpp>

namespace IG::Session {
    HikerAPI::HikerAPI(const std::string& authToken) {
        m_authToken = authToken;

        if (!validateToken())
            throw HikerError("HikerAPI token \"" + authToken + "\" is invalid");
    }

    bool HikerAPI::validateToken() {
        HikerResponse hResp = InitiateHikerRequest({
            "/sys/balance",
            "application/json",
            RequestMethod::REQ_GET,
            {},
            std::nullopt
        });
        return hResp.statusCode == 200;
    }

    HikerBalance HikerAPI::GetBalance() {
        HikerResponse hResp = InitiateHikerRequest({
            "/sys/balance",
            "application/json",
            RequestMethod::REQ_GET,
            {},
            std::nullopt
        });

        auto raw = std::get<std::string>(hResp.body.body);
        json data;
        try {
            data = json::parse(raw);
        } catch (json::parse_error& e) {
            throw HikerError("Could not parse HikerAPI json", raw);
        }

        if (!data.contains("requests"))
            throw HikerError("\"requests\" key has not been found");
        if (!data.contains("rate"))
            throw HikerError("\"rate\" key has not been found");
        if (!data.contains("currency"))
            throw HikerError("\"currency\" key has not been found");
        if (!data.contains("amount"))
            throw HikerError("\"amount\" key has not been found");

        return {
            data.at("requests"),
            data.at("rate"),
            data.at("amount"),
            data.at("currency")
        };
    }

    HikerResponse HikerAPI::InitiateHikerRequest(const HikerRequest& request) {
        std::string _ep = request.endpoint;
        if (!_ep.starts_with("/"))
            _ep = "/" + _ep;

        RequestData req;
        req.url = HIKER_API_URL_PREFIX + _ep;
        req.method = request.method;
        req.headers.emplace_back("Accept", request.accepts);
        req.headers.emplace_back("x-access-key", m_authToken);

        if (!request.extraHeaders.empty()) {
            for (const auto& eHead : request.extraHeaders) {
                req.headers.emplace_back(eHead);
            }
        }

        if (request.body.has_value()) {
            HikerBody body = request.body.value();
            req.headers.emplace_back("Content-Type", body.contentType);

            if (std::holds_alternative<std::string>(body.body))
                req.body = std::get<std::string>(body.body);
            else if (std::holds_alternative<std::vector<unsigned char>>(body.body))
                req.body = std::get<std::vector<unsigned char>>(body.body);
        }

        ResponseData response = CreateRequest(req);

        HikerResponse hResp;
        hResp.statusCode = response.statusCode;

        for (const auto& [fst, snd] : response.headers) {
            hResp.headers.emplace_back(fst, snd);

            if (std::string fstLwr = ToLowercase(fst); fstLwr == "content-type")
                hResp.body.contentType = snd;
        }

        if (std::holds_alternative<std::string>(response.body))
            hResp.body.body = std::get<std::string>(response.body);
        else if (std::holds_alternative<std::vector<unsigned char>>(response.body))
            hResp.body.body = std::get<std::vector<unsigned char>>(response.body);

        return hResp;
    }
}
