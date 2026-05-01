#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <fstream>
#include <filesystem>

#include <IGApi/Session.hpp>

#include <dev_tools/commons/Utils.hpp>

#include <sys/stat.h>

#ifdef __ANDROID__
#include <sys/system_properties.h>
#include <unistd.h>

#include <dev_tools/commons/Process.hpp>
#endif

namespace fs = std::filesystem;

class AndroidApiTranslationError : public std::runtime_error {
public:
    explicit AndroidApiTranslationError(const std::string& err, int api) : std::runtime_error(err + ": " + std::to_string(api)) {
    }
};

static const std::map<int, std::string> ANDROID_API_NAMES = {
    {1, "BASE"},
    {2, "BASE_1_1"},
    {3, "CUPCAKE"},
    {4, "DONUT"},
    {5, "ECLAIR"},
    {6, "ECLAIR_0_1"},
    {7, "ECLAIR_MR1"},
    {8, "FROYO"},
    {9, "GINGERBREAD"},
    {10, "GINGERBREAD_MR1"},
    {11, "HONEYCOMB"},
    {12, "HONEYCOMB_MR1"},
    {13, "HONEYCOMB_MR2"},
    {14, "ICE_CREAM_SANDWICH"},
    {15, "ICE_CREAM_SANDWICH_MR1"},
    {16, "JELLY_BEAN"},
    {17, "JELLY_BEAN_MR1"},
    {18, "JELLY_BEAN_MR2"},
    {19, "KITKAT"},
    {20, "KITKAT_WATCH"},
    {21, "LOLLIPOP"},
    {22, "LOLLIPOP_MR1"},
    {23, "M"},
    {24, "N"},
    {25, "N_MR1"},
    {26, "O"},
    {27, "O_MR1"},
    {28, "P"},
    {29, "Q"},
    {30, "R"},
    {31, "S"},
    {32, "S_V2"},
    {33, "TIRAMISU"},
    {34, "UPSIDE_DOWN_CAKE"},
    {35, "VANILLA_ICE_CREAM"},
    {36, "BAKLAVA"},
    {37, "CINNAMON_BUN"},
};

std::string ANDROID_TranslateApiVersion(int val) {
    if (!ANDROID_API_NAMES.contains(val))
        throw AndroidApiTranslationError("Unknown API version", val);

    return ANDROID_API_NAMES.at(val);
}

void prepareDisplayInfo() {
#ifdef __ANDROID__
    const char* c_homeDir = getenv("HOME");
    std::string displayInfoPath;
    if (c_homeDir)
        displayInfoPath = std::string(c_homeDir) + "/.local/share/OsintgramCXX/display.json";
    else
        displayInfoPath = "/data/data/com.termux/files/home/.local/share/OsintgramCXX/display.json";

    std::string classesPath = ExecutableDirectory() + "/android/classes.dex";
    if (chmod(classesPath.c_str(), 0544) != 0) {
        if (errno == EACCES) {
            std::string ncp;
            if (c_homeDir)
                ncp = std::string(c_homeDir) + "/.cache/classes.dex";
            else
                ncp = "/data/data/com.termux/files/home/.cache/classes.dex";

            std::ofstream ofd(ncp, std::ios::binary);
            if (!ofd.is_open())
                throw std::runtime_error("Unable to obtain display information (could not make a copy of classes.dex)");

            std::ifstream ifd(ExecutableDirectory() + "/android/classes.dex", std::ios::binary);
            if (!ifd.is_open())
                throw std::runtime_error("Unable to obtain display information (could not make a copy of classes.dex)");

            ofd << ifd.rdbuf();
            ifd.close();
            ofd.close();

            if (chmod(ncp.c_str(), 0544) != 0) {
                throw std::runtime_error(
                    "Unable to obtain display information - classes.dex copy chmod: " + std::string(
                        std::strerror(errno)));
            }

            classesPath = std::string(ncp);
        } else
            throw std::runtime_error("Unable to prepare classes.dex: " + std::string(std::strerror(errno)));
    }

    ProcessRequest req;
    req.binaryPath = "/system/bin/app_process";
    req.env_map["CLASSPATH"] = classesPath;
    req.args.emplace_back("/system/bin");
    req.args.emplace_back("net.bc100dev.osintgram.Main");
    req.args.emplace_back("display-info");
    req.args.emplace_back(displayInfoPath);
    req.redirectAllIoToSelf = true;

    try {
        if (int rc = SpawnProcess(req); rc != 0)
            std::cerr << "Unable to save display info: returned exit code " << rc << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Unable to save display info: " << ex.what() << std::endl;
    }
#endif
}

namespace IG::Session {
    const std::string& Device::KEYINFO_ANDROID_API = "android_api";
    const std::string& Device::KEYINFO_ANDROID_VERSION = "android_codebase";
    const std::string& Device::KEYINFO_DISPLAY_DPI = "display_dpi";
    const std::string& Device::KEYINFO_DISPLAY_WIDTH = "display_width";
    const std::string& Device::KEYINFO_DISPLAY_HEIGHT = "display_height";
    const std::string& Device::KEYINFO_DEVICE_PRODUCT = "product";
    const std::string& Device::KEYINFO_DEVICE_MANUFACTURER = "manufacturer";
    const std::string& Device::KEYINFO_DEVICE_CODENAME = "codename";
    const std::string& Device::KEYINFO_DEVICE_CPU_LABEL = "cpu_label";

    std::string Device::MakeUserAgent() {
        return "";
    }

    Device::Type Device::GetDeviceType() {
        return m_deviceType;
    }

    void Device::SetInfo(const std::string& key, const std::any& val) {
        deviceInfo[key] = val;

        if (m_deviceType == Type::MOBILE) {
            bool typeInt = val.type() == typeid(int);
            bool typeStr = val.type() == typeid(std::string);

            try {
                if (key == KEYINFO_ANDROID_API && typeInt) {
                    m_androidInfo.androidVersion.apiVersion = CastData<int>(val);
                    m_androidInfo.androidVersion.codebaseVersion = ANDROID_TranslateApiVersion(CastData<int>(val));
                } else if (key == KEYINFO_DISPLAY_DPI && typeInt) {
                    m_androidInfo.displaySize.dpi = CastData<int>(val);
                } else if ((key == KEYINFO_DISPLAY_WIDTH || key == "width") && typeInt) {
                    m_androidInfo.displaySize.width = CastData<int>(val);
                } else if ((key == KEYINFO_DISPLAY_HEIGHT || key == "height") && typeInt) {
                    m_androidInfo.displaySize.height = CastData<int>(val);
                } else if ((key == KEYINFO_DEVICE_MANUFACTURER || key == "manufacturer") && typeStr) {
                    m_androidInfo.manufacturer = CastData<std::string>(val);
                } else if ((key == KEYINFO_DEVICE_PRODUCT || key == "product") && typeStr) {
                    m_androidInfo.product = CastData<std::string>(val);
                } else if ((key == KEYINFO_DEVICE_CODENAME || key == "codename") && typeStr) {
                    m_androidInfo.codename = CastData<std::string>(val);
                } else if ((key == KEYINFO_DEVICE_CPU_LABEL || key == "cpu_label") && typeStr) {
                    m_androidInfo.cpuLabel = CastData<std::string>(val);
                }
            } catch (const std::bad_any_cast& ex) {
                throw std::runtime_error("Value type invalid (err = " + std::string(ex.what()) + ")");
            }
        }
    }

    std::any Device::GetInfo(const std::string& key) {
        if (m_deviceType == Type::MOBILE) {
            if (key == KEYINFO_ANDROID_API)
                return m_androidInfo.androidVersion.apiVersion;

            if (key == KEYINFO_ANDROID_VERSION)
                return m_androidInfo.androidVersion.codebaseVersion;

            if (key == KEYINFO_DISPLAY_DPI)
                return m_androidInfo.displaySize.dpi;

            if (key == KEYINFO_DISPLAY_WIDTH || key == "width")
                return m_androidInfo.displaySize.width;

            if (key == KEYINFO_DISPLAY_HEIGHT || key == "height")
                return m_androidInfo.displaySize.height;

            if (key == KEYINFO_DEVICE_MANUFACTURER || key == "manufacturer")
                return m_androidInfo.manufacturer;

            if (key == KEYINFO_DEVICE_PRODUCT || key == "product")
                return m_androidInfo.product;

            if (key == KEYINFO_DEVICE_CODENAME || key == "codename")
                return m_androidInfo.codename;

            if (key == KEYINFO_DEVICE_CPU_LABEL || key == "cpu_label")
                return m_androidInfo.cpuLabel;
        }

        if (auto it = deviceInfo.find(key); it != deviceInfo.end())
            return it->second;

        throw std::runtime_error("Key " + key + " not found");
    }

    Device::OperatingSystem Device::GetOperatingSystem() {
        return m_os;
    }

    void Device::SetOperatingSystem(const OperatingSystem& os) {
        m_os = os;
    }

    Device CopyHostDeviceSetup() {
        Device device;

#ifdef __ANDROID__
        const char* homeDir = getenv("HOME");
        if (homeDir == nullptr)
            throw std::runtime_error("HOME not set");

        fs::create_directory(fs::path(std::string(homeDir) + "/.local/share/OsintgramCXX"));

        if (!fs::exists(std::string(homeDir) + "/.local/share/OsintgramCXX/display.json"))
            prepareDisplayInfo();

        std::ifstream displayInfo(std::string(homeDir) + "/.local/share/OsintgramCXX/display.json");
        int w, h, d;
        if (!displayInfo.is_open())
            throw std::runtime_error("Could not open display info file");

        nlohmann::json j;
        displayInfo >> j;

        w = j["width"].get<int>();
        h = j["height"].get<int>();
        d = j["dpi"].get<int>();

        displayInfo.close();

        device = Device(Device::Type::MOBILE);

        // System properties
        char buf[PROP_VALUE_MAX];

        __system_property_get("ro.build.version.sdk", buf);
        int api = std::stoi(buf);

        __system_property_get("ro.product.manufacturer", buf);
        std::string manufacturer = buf;

        __system_property_get("ro.product.model", buf);
        std::string product = buf;

        __system_property_get("ro.product.device", buf);
        std::string codename = buf;

        __system_property_get("ro.board.platform", buf);
        std::string cpu = buf;

        device.SetInfo(Device::KEYINFO_ANDROID_API, api);
        device.SetInfo(Device::KEYINFO_DEVICE_CPU_LABEL, cpu);
        device.SetInfo(Device::KEYINFO_DEVICE_MANUFACTURER, manufacturer);
        device.SetInfo(Device::KEYINFO_DEVICE_PRODUCT, product);
        device.SetInfo(Device::KEYINFO_DEVICE_CODENAME, codename);
        device.SetInfo(Device::KEYINFO_DISPLAY_DPI, d);
        device.SetInfo(Device::KEYINFO_DISPLAY_HEIGHT, h);
        device.SetInfo(Device::KEYINFO_DISPLAY_WIDTH, w);
#else
        device = Device(Device::Type::DESKTOP);
#endif

        return device;
    }
}
