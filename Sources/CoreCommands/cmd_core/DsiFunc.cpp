#include "FuncHeaders.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <variant>

#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/commons/HelpPage.hpp>
#include <dev_tools/network/Network.hpp>

#include <IGApi/Session.hpp>

#include "dsi/DSI.hpp"

using namespace DevTools;
using namespace IG::Session;

namespace fs = std::filesystem;

namespace DSI_Data {
    struct ListInf {
        std::optional<size_t> maxCount = std::nullopt;
        size_t startAt = 0;
    };

    std::variant<int, ListInf> ParseListCmd(const std::vector<std::string>& args, const std::vector<Device>& devList) {
        ListInf inf;

        for (size_t i = 1; i < args.size(); i++) {
            const std::string& arg = args[i];
            if (arg.starts_with("--max") || arg.starts_with("--max-size")) {
                size_t eqp = arg.find('=');
                if (eqp == std::string::npos) {
                    std::cerr << arg << ": malformed value, '=' missing" << std::endl;
                    return 1;
                }

                std::string val = arg.substr(eqp + 1);
                try {
                    // probe
                    size_t tmp = std::stol(val);
                    inf.maxCount = tmp;
                } catch (...) {
                    std::cerr << "cannot convert " << val << " (not valid number)" << std::endl;
                    return 1;
                }
            }

            if (arg.starts_with("--start") || arg.starts_with("--start-at")) {
                size_t eqp = arg.find('=');
                if (eqp == std::string::npos) {
                    std::cerr << arg << ": malformed value, '=' missing" << std::endl;
                    return 1;
                }

                std::string val = arg.substr(eqp + 1);
                try {
                    // probe
                    size_t tmp = std::stol(val);
                    if (tmp >= devList.size()) {
                        std::cerr << "Starting position " << tmp << " is outside the device list range." << std::endl;
                        return 1;
                    }

                    inf.startAt = tmp;
                } catch (...) {
                    std::cerr << "cannot convert " << val << " (not valid number)" << std::endl;
                    return 1;
                }
            }
        }

        return inf;
    }
}

int HostInfoCopy() {
    Device device = CopyHostDeviceSetup();
#if NDEBUG
    std::cout << "Display Height: " << std::any_cast<int>(device.GetInfo(Device::KEYINFO_DISPLAY_HEIGHT)) << std::endl;
    std::cout << "Display Width: " << std::any_cast<int>(device.GetInfo(Device::KEYINFO_DISPLAY_WIDTH)) << std::endl;
    std::cout << "Display DPI: " << std::any_cast<int>(device.GetInfo(Device::KEYINFO_DISPLAY_DPI)) << std::endl;
    std::cout << "Manufacturer: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_DEVICE_MANUFACTURER)) <<
        std::endl;
    std::cout << "Product: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_DEVICE_PRODUCT)) << std::endl;
    std::cout << "Codenase: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_DEVICE_CODENAME)) <<
        std::endl;
    std::cout << "CPU label: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_DEVICE_CPU_LABEL)) <<
        std::endl;
    std::cout << "Android API: " << std::any_cast<int>(device.GetInfo(Device::KEYINFO_ANDROID_API)) << std::endl;
    std::cout << "Android Version: " << std::any_cast<std::string>(device.GetInfo(Device::KEYINFO_ANDROID_VERSION)) <<
        std::endl;
#endif

    SetCurrentDevice(device);
    return 0;
}

int dsi_func(const std::vector<std::string>& args, const std::map<std::string, std::string>& env) {
    if (args.empty() || args[0] == "help" || args[0] == "h" || args[0] == "--help" || args[0] == "-h") {
        std::cout << "Usage: dsi <action> (value) <flags>" << std::endl << std::endl;
        std::cout << "Actions:" << std::endl;

        HelpPage actions;
        actions.addArg("update", std::nullopt, "Updates the device profiles");
        actions.addArg("set-device", std::nullopt,
                       "Sets a device for DSI assigned to the Instagram APIs using the index of a device");
        actions.addArg("get-device", std::nullopt, "Gets the currently assigned device passed via DSI");
        //actions.addArg("contribute-host", std::nullopt,
        //               "Contribute your own host device details for the device listing");
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

    if (args[0] == "update")
        return DSI::Update();

    std::vector<Device> devList;
    try {
        devList = DSI::GetDeviceListing();
    } catch (const std::exception& ex) {
        std::cerr << "Something went wrong while attempting to fetch device listing" << std::endl;
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    if (args[0] == "set-device") {
        if (args[1] == "host")
            return HostInfoCopy();

        size_t pos;
        try {
            pos = std::stoi(args[1]);
        } catch (...) {
            std::cerr << "Malformed index given" << std::endl;
            std::cerr << "Index must be somewhere between 0 and " << devList.size() - 1 << std::endl;
            return 1;
        }

        Device& dev = devList.at(pos);
        SetCurrentDevice(dev);

        if (dev.GetDeviceType() == Device::Type::MOBILE) {
            std::cout << "Now set device to [Mobile]" << std::endl;
            std::cout << "Manufacturer: "
                << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_MANUFACTURER)) << std::endl;
            std::cout << "Product: "
                << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_PRODUCT)) << std::endl;
            std::cout << "Android version: "
                << std::any_cast<int>(dev.GetInfo(Device::KEYINFO_ANDROID_API)) << "/"
                << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_ANDROID_VERSION)) << " (codenamed "
                << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_ANDROID_CODEBASE)) << ")"
                << std::endl;
            std::cout << "Device codename: "
                << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_CODENAME)) << std::endl;
        }

        if (dev.GetDeviceType() == Device::Type::DESKTOP) {
            std::cout << "Now set device to [Desktop]" << std::endl;
            std::cout << "Operating system: ";
            switch (dev.GetOperatingSystem()) {
            case Device::OperatingSystem::WINDOWS:
                std::cout << "Windows" << std::endl;
                break;
            case Device::OperatingSystem::MAC_OS:
                std::cout << "macOS" << std::endl;
                break;
            case Device::OperatingSystem::LINUX:
                std::cout << "Linux" << std::endl;
                break;
            case Device::OperatingSystem::ANDROID:
                std::cout << "Android" << std::endl;
                break;
            case Device::OperatingSystem::IOS:
                std::cout << "iOS" << std::endl;
                break;
            default:
                std::cout << "[Unknown OS]" << std::endl;
                break;
            }

            std::cout << "Assigned Firefox Browser version: "
                << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_BROWSER_VERSION)) << std::endl;
        }

        return 0;
    }

    if (args[0] == "get-device") {
        if (const auto& optDev = GetCurrentDevice(); optDev.has_value()) {
            std::cout << "Currently assigned device type: ";
            if (optDev->GetDeviceType() == Device::Type::MOBILE) {
                std::cout << "Mobile" << std::endl;
                std::cout << "Android API version: "
                    << std::any_cast<int>(optDev->GetInfo(Device::KEYINFO_ANDROID_API)) << "/"
                    << std::any_cast<std::string>(optDev->GetInfo(Device::KEYINFO_ANDROID_VERSION)) << " (codenamed "
                    << std::any_cast<std::string>(optDev->GetInfo(Device::KEYINFO_ANDROID_CODEBASE)) << ")"
                    << std::endl;

                std::cout << "Display Metrics:" << std::endl;
                std::cout << "\tWidth: "
                    << std::any_cast<int>(optDev->GetInfo(Device::KEYINFO_DISPLAY_WIDTH)) << std::endl;
                std::cout << "\tHeight: "
                    << std::any_cast<int>(optDev->GetInfo(Device::KEYINFO_DISPLAY_HEIGHT)) << std::endl;
                std::cout << "\tDPI: "
                    << std::any_cast<int>(optDev->GetInfo(Device::KEYINFO_DISPLAY_DPI)) << "\n" << std::endl;

                std::cout << "Manufacturer: "
                    << std::any_cast<std::string>(optDev->GetInfo(Device::KEYINFO_DEVICE_MANUFACTURER)) << std::endl;
                std::cout << "Product: "
                    << std::any_cast<std::string>(optDev->GetInfo(Device::KEYINFO_DEVICE_PRODUCT)) << std::endl;
                std::cout << "Device codename: "
                    << std::any_cast<std::string>(optDev->GetInfo(Device::KEYINFO_DEVICE_CODENAME)) << std::endl;
                std::cout << "CPU label: "
                    << std::any_cast<std::string>(optDev->GetInfo(Device::KEYINFO_DEVICE_CPU_LABEL)) << std::endl;
            }

            if (optDev->GetDeviceType() == Device::Type::DESKTOP) {
                std::cout << "Desktop" << std::endl;
                std::cout << "Operating system: ";
                switch (optDev->GetOperatingSystem()) {
                case Device::OperatingSystem::WINDOWS:
                    std::cout << "Windows" << std::endl;
                    break;
                case Device::OperatingSystem::MAC_OS:
                    std::cout << "macOS" << std::endl;
                    break;
                case Device::OperatingSystem::LINUX:
                    std::cout << "Linux" << std::endl;
                    break;
                case Device::OperatingSystem::ANDROID:
                    std::cout << "Android" << std::endl;
                    break;
                case Device::OperatingSystem::IOS:
                    std::cout << "iOS" << std::endl;
                    break;
                default:
                    std::cout << "[Unknown OS]" << std::endl;
                    break;
                }

                std::cout << "Assigned Firefox Browser version: "
                    << std::any_cast<std::string>(optDev->GetInfo(Device::KEYINFO_BROWSER_VERSION)) << std::endl;
            }

            if (env.contains("DumpUserAgent") && env.at("DumpUserAgent") == "1")
                std::cout << "User Agent: " << optDev->MakeUserAgent() << std::endl;

            return 0;
        }

        std::cerr << "No device is currently assigned." << std::endl;
        return 1;
    }

    if (args[0] == "list") {
        std::variant<int, DSI_Data::ListInf> vInf = DSI_Data::ParseListCmd(args, devList);
        if (std::holds_alternative<int>(vInf))
            return std::get<int>(vInf);

        auto inf = std::get<DSI_Data::ListInf>(vInf);
        size_t counter = 0;

        for (size_t i = inf.startAt; i < devList.size(); i++) {
            if (inf.maxCount.has_value() && inf.maxCount > 0 && counter >= inf.maxCount)
                break;

            const Device& dev = devList[i];
            std::cout << i << ": ";

            if (dev.GetDeviceType() == Device::Type::MOBILE) {
                std::cout << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_MANUFACTURER)) << " ";
                std::cout << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_PRODUCT)) << " (v";
                std::cout << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_ANDROID_VERSION)) << "/";
                std::cout << std::any_cast<int>(dev.GetInfo(Device::KEYINFO_ANDROID_API)) << ", codenamed ";
                std::cout << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_CODENAME)) << ")";
                std::cout << " -- Mobile";
            }

            if (dev.GetDeviceType() == Device::Type::DESKTOP) {
                std::string osLabel;
                switch (dev.GetOperatingSystem()) {
                case Device::OperatingSystem::WINDOWS:
                    osLabel = "Windows";
                    break;
                case Device::OperatingSystem::MAC_OS:
                    osLabel = "MacOS";
                    break;
                case Device::OperatingSystem::LINUX:
                    osLabel = "Linux";
                    break;
                case Device::OperatingSystem::ANDROID:
                    osLabel = "Android";
                    break;
                case Device::OperatingSystem::IOS:
                    osLabel = "iOS";
                    break;
                default:
                    osLabel = "[Unknown OS]";
                    break;
                }

                std::cout << osLabel << " (Browser Version ";
                std::cout << std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_BROWSER_VERSION)) << ")";
                std::cout << " -- Desktop";
            }

            std::cout << std::endl;
            counter++;
        }

        //std::cout << "\n";

        return 0;
    }

    std::cerr << "Unknown command given: " << args[0] << std::endl;
    return 1;
}
