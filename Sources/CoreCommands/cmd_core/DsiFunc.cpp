#include "FuncHeaders.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>

#include <OsintgramCXX/Networking/Networking.hpp>

#include <OsintgramCXX/Commons/Utils.hpp>
#include <OsintgramCXX/Commons/HelpPage.hpp>

using namespace OsintgramCXX::Networking;

namespace fs = std::filesystem;

void update() {
    RequestData req;
    req.url = "https://raw.githubusercontent.com/BC100Dev/OsintgramCXX/refs/heads/master/Resources/device_profiles.json";
    req.method = REQ_GET;
    req.headers.emplace_back("User-Agent", "GitHub (OsintgramCXX, v1.0)");

    std::string jsonData;
    {
        ResponseData res = CreateRequest(req);

        for (char c : res.data)
            jsonData += c;
    }

    std::string userConfig;
#ifdef __linux__
    userConfig = std::string(std::getenv("HOME")) + "/.local/share/OsintgramCXX/device_profiles.json";
#elif defined(_WIN32)
    userConfig = std::string(std::getenv("LOCALAPPDATA")) + "\\OsintgramCXX\\device_profiles.json";
#endif

    if (!fs::exists(userConfig)) {
        try {
            OsintgramCXX::CreateFile(userConfig);
        } catch (const std::runtime_error& err) {
            std::cerr << "Unable to update the device_profiles.json" << std::endl;
            std::cerr << "Cause: " << err.what() << std::endl;
            return;
        }
    }

    std::ofstream of(userConfig);
    if (of.is_open()) {
        of << jsonData << "\n";
        of.close();
    } else {
        std::cerr << "Unable to write data to \"device_profiles.json\"" << std::endl;

#ifdef __linux__
        if (errno != 0)
            std::cerr << "Cause for the error: " << errno << std::endl;
#elif defined(_WIN32)
        DWORD err = GetLastError();
        if (err != 0)
            std::cerr << "Cause for the error: 0x" << std::hex << err << std::endl;
#endif
    }
}

int dsi_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env) {
    if (args.empty()) {
        std::cout << "Usage: dsi <action> (value) <flags>" << std::endl << std::endl;
        std::cout << "Actions:" << std::endl;

        HelpPage actions;
        actions.addArg("update", "", "Updates the device profiles");
        actions.addArg("enable", "", "Enables a device that can be ready for use");
        actions.addArg("disable", "", "Disables a device from spoofing Instagram");
        actions.addArg("set-device", "", "Sets a device for DSI assigned to the Instagram APIs");
        actions.addArg("get-device", "", "Gets the currently assigned device passed via DSI");
        actions.setStartSpaceWidth(3);
        actions.setSpaceWidth(3);
        actions.display(std::cout);
    }

    return 0;
}