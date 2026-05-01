#include "FuncHeaders.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>

#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/commons/HelpPage.hpp>
#include <dev_tools/network/Network.hpp>

#include "IGApi/Session.hpp"

using namespace DevTools;
using namespace IG::Session;

namespace fs = std::filesystem;

void update() {
    RequestData req;
    req.url =
        "https://raw.githubusercontent.com/BC100Dev/OsintgramCXX/refs/heads/master/Resources/device_profiles.json";
    req.method = RequestMethod::REQ_GET;
    req.headers.emplace_back("User-Agent", "GitHub (OsintgramCXX, v1.0)");
    ResponseData res = CreateRequest(req);

    std::string jsonData = ReadByteData(res.body);
    std::string userConfig;
#ifdef __linux__
    userConfig = std::string(std::getenv("HOME")) + "/.local/share/OsintgramCXX/device_profiles.json";
#elif defined(_WIN32)
    userConfig = std::string(std::getenv("LOCALAPPDATA")) + "\\OsintgramCXX\\device_profiles.json";
#endif

    if (!fs::exists(userConfig)) {
        try {
            CreateFile(userConfig);
        } catch (const std::runtime_error& err) {
            std::cerr << "Unable to update the device_profiles.json" << std::endl;
            std::cerr << "Cause: " << err.what() << std::endl;
            return;
        }
    }

    if (std::ofstream of(userConfig); of.is_open()) {
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

int HostInfoCopy() {
    Device device = CopyHostDeviceSetup();
    std::cout << "Display Height: " << std::any_cast<int>(device.GetInfo(Device::KEYINFO_DISPLAY_HEIGHT)) << std::endl;
    std::cout << "Display Width: " << std::any_cast<int>(device.GetInfo(Device::KEYINFO_DISPLAY_WIDTH)) << std::endl;
    std::cout << "Display DPI: " << std::any_cast<int>(device.GetInfo(Device::KEYINFO_DISPLAY_DPI)) << std::endl;
    std::cout << "Manufacturer: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_DEVICE_MANUFACTURER)) << std::endl;
    std::cout << "Product: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_DEVICE_PRODUCT)) << std::endl;
    std::cout << "Codenase: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_DEVICE_CODENAME)) << std::endl;
    std::cout << "CPU label: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_DEVICE_CPU_LABEL)) << std::endl;
    std::cout << "Android API: " << std::any_cast<int>(device.GetInfo(Device::KEYINFO_ANDROID_API)) << std::endl;
    std::cout << "Android Version: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_ANDROID_VERSION)) << std::endl;
    return 0;
}

int dsi_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env) {
    if (args.empty() || args[0] == "help" || args[0] == "h" || args[0] == "--help" || args[0] == "-h") {
        std::cout << "Usage: dsi <action> (value) <flags>" << std::endl << std::endl;
        std::cout << "Actions:" << std::endl;

        HelpPage actions;
        actions.addArg("update", std::nullopt, "Updates the device profiles");
        actions.addArg("enable", std::nullopt, "Enables a device that can be ready for use");
        actions.addArg("disable", std::nullopt, "Disables a device from spoofing Instagram");
        actions.addArg("set-device", std::nullopt, "Sets a device for DSI assigned to the Instagram APIs");
        actions.addArg("get-device", std::nullopt, "Gets the currently assigned device passed via DSI");
        actions.addArg("list", std::nullopt, "Lists all available devices");
        actions.setStartSpaceWidth(3);
        actions.setSpaceWidth(3);
        actions.display(std::cout);

        std::cout << "\nSetting a device with param \"host\" will include the host device information." << std::endl;
        std::cout << "This will populate the necessary device information, which includes:" << std::endl;

#ifdef __ANDROID__
        std::cout << "  - Display Metrics (width, height, DPI)" << std::endl;
        std::cout << "  - Device manufacturer, product and codename (e.g. Pixel 8 Pro for husky)" << std::endl;
        std::cout << "  - Device CPU label" << std::endl;
        std::cout << "  - Device product" << std::endl;
        std::cout << "  - Android API/SDK version" << std::endl;
#endif

        std::cout << "  - Operating System itself" << std::endl;
        std::cout << "  - The primary system architecture (e.g. arm64-v8a)" << std::endl << std::endl;

        std::cout << "To see flags for each action, type \"[action] --flags\"." << std::endl;
        return 0;
    }

    if (args[0] == "update") {
    } else if (args[0] == "enable") {
    } else if (args[0] == "disable") {
    } else if (args[0] == "set-device") {
        if (args[1] == "host")
            return HostInfoCopy();
    } else if (args[0] == "get-device") {
    } else if (args[0] == "list") {
    }

    return 0;
}
