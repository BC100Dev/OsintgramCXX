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
            .endpoint = "/sys/balance",
            .accepts = "application/json",
            .method = RequestMethod::REQ_GET,
            .extraHeaders = {},
            .body = std::nullopt
        });

        auto raw = std::string(hResp.body.body.begin(), hResp.body.body.end());
        json data;
        try {
            data = json::parse(raw);
        } catch (json::parse_error&) {
            throw HikerError("Could not parse HikerAPI json", raw);
        }

        // haha unhandled errors go BRRRRRRRR
        RequireJsonKeys(data, {
                            "requests",
                            "rate",
                            "currency",
                            "amount"
                        }, "HikerAPI Error");

        return {
            .requests = data.at("requests"),
            .rate = data.at("rate"),
            .amount = data.at("amount"),
            .currency = data.at("currency")
        };
    }

    HikerResponse HikerAPI::InitiateHikerRequest(const HikerRequest& request) {
        // I need therapy rn 😭
        std::string haveYouReceivedTheErection = request.endpoint;
        if (!haveYouReceivedTheErection.starts_with('/'))
            haveYouReceivedTheErection.insert(0, 1, '/');

        // have you?

        RequestData req;
        req.url = HIKER_API_URL_PREFIX + haveYouReceivedTheErection;
        req.method = request.method;
        req.headers.emplace_back("Accept", request.accepts);
        req.headers.emplace_back("x-access-key", m_authToken);

        if (!request.extraHeaders.empty()) {
            for (const auto& eHead : request.extraHeaders) {
                req.headers.emplace_back(eHead);
            }
        }

        if (request.body.has_value()) {
            req.headers.emplace_back("Content-Type", request.body->contentType);
            req.body = request.body->body;
        }

        ResponseData response = CreateRequest(req);

        HikerResponse hResp;
        hResp.statusCode = response.statusCode;

        for (const auto& [fst, snd] : response.headers) {
            hResp.headers.emplace_back(fst, snd);

            if (std::string fstLwr = ToLowercase(fst); fstLwr == "content-type")
                hResp.body.contentType = snd;
        }
        hResp.body.body = response.body;

        return hResp;
    }
}
