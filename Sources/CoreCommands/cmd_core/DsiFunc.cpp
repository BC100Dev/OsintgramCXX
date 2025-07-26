#include "FuncHeaders.hpp"

#include <iostream>
#include <filesystem>

#include <OsintgramCXX/Networking/Networking.hpp>

using namespace OsintgramCXX::Networking;

int dsi_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env) {
    RequestData req;
    req.url = "https://raw.githubusercontent.com/BC100Dev/OsintgramCXX/refs/heads/master/Resources/device_profiles.json";
    req.method = GET;
    req.headers.emplace_back("User-Agent", "GitHub (OsintgramCXX, v1.0)");

    std::string jsonData;
    {
        ResponseData res = CreateRequest(req);

        for (char c : res.data)
            jsonData += c;
    }

    return 0;
}