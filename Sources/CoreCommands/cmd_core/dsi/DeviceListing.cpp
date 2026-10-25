#include "DSI.hpp"

#include <fstream>

#include <dev_tools/commons/Paths.hpp>
#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/network/Network.hpp>

#define DSI_DEFAULT_REMOTE "https://raw.githubusercontent.com/BC100Dev/OsintgramCXX/refs/heads/master/Resources/device_profiles.json"

using namespace App::Paths;
using namespace IG::Session;

namespace DSI {
    std::vector<Device> inMemDeviceList{};

    const auto DefaultDeviceListingStoreLocation = fs::path(UserConfig() / "OsintgramCXX" / "devices.json");

    std::string FetchRawDataDeviceListing(const std::string& source) {
        if (source.starts_with("http://") || source.starts_with("https://")) {
            RequestData req;
            req.url = source;
            req.followRedirects = true;
            req.headers.emplace_back("Accept", "application/json; charset=utf-8");
            req.headers.emplace_back("User-Agent", "OSS (BC100Dev/OsintgramCXX v1)");

            ResponseData resp = CreateRequest(req);
            if (resp.statusCode != 200)
                throw std::runtime_error("Remote source has been given a non 200 status code");

            return {resp.body.begin(), resp.body.end()};
        }

        if (!fs::exists(source))
            throw std::runtime_error(std::format("Source at {} does not exist", source));

        std::ifstream ifs(source);
        if (!ifs.is_open())
            throw std::runtime_error(std::format("Could not open source at {}", source));

        std::string content((std::istreambuf_iterator(ifs)),
                            std::istreambuf_iterator<char>());
        ifs.close();

        return content;
    }

    Device Mobile2Dev(const json& obj) {
        RequireJsonKeys(obj, {
                            "android",
                            "manufacturer",
                            "product",
                            "codename",
                            "cpu_label",
                            "display_size"
                        });

        json androidObj = obj["android"].get<json::object_t>();
        json displayObj = obj["display_size"].get<json::object_t>();

        RequireJsonKeys(androidObj, {"api", "version"});
        RequireJsonKeys(displayObj, {"width", "height", "dpi"});

        Device dev(Device::Type::MOBILE);
        dev.SetOperatingSystem(Device::OperatingSystem::ANDROID);
        dev.SetInfo(Device::KEYINFO_ANDROID_API, androidObj["api"].get<int>());
        dev.SetInfo(Device::KEYINFO_DISPLAY_HEIGHT, displayObj["height"].get<int>());
        dev.SetInfo(Device::KEYINFO_DISPLAY_WIDTH, displayObj["width"].get<int>());
        dev.SetInfo(Device::KEYINFO_DISPLAY_DPI, displayObj["dpi"].get<int>());
        dev.SetInfo(Device::KEYINFO_DEVICE_MANUFACTURER, obj["manufacturer"].get<std::string>());
        dev.SetInfo(Device::KEYINFO_DEVICE_PRODUCT, obj["product"].get<std::string>());
        dev.SetInfo(Device::KEYINFO_DEVICE_CODENAME, obj["codename"].get<std::string>());
        dev.SetInfo(Device::KEYINFO_DEVICE_CPU_LABEL, obj["cpu_label"].get<std::string>());

        return dev;
    }

    Device Desktop2Dev(const json& obj) {
        RequireJsonKeys(obj, {"os_label", "browser_version"});
        auto osLabel = ToLowercase(obj["os_label"].get<std::string>());

        Device::OperatingSystem os;
        if (osLabel == "win" || osLabel == "win32" || osLabel == "win64" || osLabel.contains("windows"))
            os = Device::OperatingSystem::WINDOWS;
        else if (osLabel == "linux")
            os = Device::OperatingSystem::LINUX;
        else if (osLabel == "mac" || osLabel == "macos" || osLabel == "macosx" || osLabel == "osx")
            os = Device::OperatingSystem::MAC_OS;
        else
            throw std::runtime_error(std::format("Unknown OS label {}", osLabel));

        Device dev(Device::Type::DESKTOP);
        dev.SetOperatingSystem(os);
        dev.SetInfo(Device::KEYINFO_BROWSER_VERSION, obj["browser_version"].get<std::string>());
        return dev;
    }

    std::vector<Device> GetDeviceListing(bool local) {
        std::vector<Device> deviceList;
        std::string sourceLoc = local
                                    ? DefaultDeviceListingStoreLocation.string()
                                    : DSI_DEFAULT_REMOTE;
        std::string raw = FetchRawDataDeviceListing(sourceLoc);
        json data = json::parse(raw);
        if (!data.is_array())
            throw std::runtime_error("Expected JSON format to have array in root, returned JSON object");

        for (const auto& obj : data) {
            RequireJsonKeys(obj, {"type"});
            if (obj["type"].get<std::string>() == "mobile") {
                deviceList.push_back(Mobile2Dev(obj));
                continue;
            }

            if (obj["type"].get<std::string>() == "desktop") {
                deviceList.push_back(Desktop2Dev(obj));
                continue;
            }

            throw std::runtime_error(std::format("Unexpected JSON type in device listing, received {}",
                                                 obj["type"].get<std::string>()));
        }

        return deviceList;
    }

    std::vector<Device> GetDeviceListing() {
        if (!inMemDeviceList.empty())
            return inMemDeviceList;

        if (fs::exists(DefaultDeviceListingStoreLocation))
            return GetDeviceListing(true);

        std::cout << "Local device listing file not found; populating" << std::endl;

        std::vector<Device> devList = GetDeviceListing(false);
        SaveDeviceListing(devList);

        inMemDeviceList.clear();
        inMemDeviceList.shrink_to_fit();
        inMemDeviceList.insert(inMemDeviceList.end(), devList.begin(), devList.end());
        return devList;
    }

    void SaveDeviceListing(const std::vector<Device>& deviceList) {
        if (!fs::exists(DefaultDeviceListingStoreLocation.parent_path()))
            fs::create_directories(DefaultDeviceListingStoreLocation.parent_path());

        if (!fs::exists(DefaultDeviceListingStoreLocation))
            MkFile(DefaultDeviceListingStoreLocation);

        json writer = json::array();
        for (const auto& dev : deviceList) {
            json obj = json::object();

            if (dev.GetDeviceType() == Device::Type::DESKTOP) {
                obj["type"] = "desktop";
                std::string osLabel;
                switch (dev.GetOperatingSystem()) {
                case Device::OperatingSystem::WINDOWS: osLabel = "windows";
                    break;
                case Device::OperatingSystem::LINUX: osLabel = "linux";
                    break;
                case Device::OperatingSystem::MAC_OS: osLabel = "macos";
                    break;
                default: osLabel = "unknown";
                    break;
                }

                obj["os_label"] = osLabel;
                obj["browser_version"] = std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_BROWSER_VERSION));
            } else if (dev.GetDeviceType() == Device::Type::MOBILE) {
                obj["type"] = "mobile";
                obj["manufacturer"] = std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_MANUFACTURER));
                obj["product"] = std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_PRODUCT));
                obj["codename"] = std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_CODENAME));
                obj["cpu_label"] = std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_DEVICE_CPU_LABEL));
                obj["android"] = {
                    {"api", std::any_cast<int>(dev.GetInfo(Device::KEYINFO_ANDROID_API))},
                    {"version", std::any_cast<std::string>(dev.GetInfo(Device::KEYINFO_ANDROID_VERSION))}
                };
                obj["display_size"] = {
                    {"width", std::any_cast<int>(dev.GetInfo(Device::KEYINFO_DISPLAY_WIDTH))},
                    {"height", std::any_cast<int>(dev.GetInfo(Device::KEYINFO_DISPLAY_HEIGHT))},
                    {"dpi", std::any_cast<int>(dev.GetInfo(Device::KEYINFO_DISPLAY_DPI))}
                };
            }

            writer.push_back(obj);
        }

        std::ofstream ofs(DefaultDeviceListingStoreLocation);
        if (!ofs.is_open())
            throw std::runtime_error(std::format("Could not open file {}: {}",
                                                 DefaultDeviceListingStoreLocation.string(),
                                                 ConstructErrorMessage()));

        ofs << writer.dump(4);
        ofs.close();
    }

    int Update() {
        try {
            std::vector<Device> devList = GetDeviceListing(false);
            SaveDeviceListing(devList);

            inMemDeviceList.clear();
            inMemDeviceList.shrink_to_fit();
            inMemDeviceList.insert(inMemDeviceList.end(), devList.begin(), devList.end());
            return 0;
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
}
