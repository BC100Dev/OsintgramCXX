#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <filesystem>

#include <IGApi/Session.hpp>

#include <dev_tools/commons/Utils.hpp>

#include <sys/stat.h>

#if __has_include(<OsintgramCXX/App/AppProps.hpp>)
#include <OsintgramCXX/App/AppProps.hpp>
#endif

#ifdef __ANDROID__
#include <sys/system_properties.h>

#include <dev_tools/commons/Process.hpp>
#else

#define BROWSER_DEFAULT_VERSION "150.0"

#endif

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

class AndroidApiTranslationError : public std::runtime_error {
public:
    explicit AndroidApiTranslationError(int api) : std::runtime_error(
        "Unknown Android API provided: " + std::to_string(api)) {
    }
};

static const std::vector<IG::Session::Device::AndroidVersion> ANDROID_API_MAP = {
    {1, "1.0", "BASE"},
    {2, "1.1", "BASE_1_1"},
    {3, "1.5", "CUPCAKE"},
    {4, "1.6", "DONUT"},
    {5, "2.0", "ECLAIR"},
    {6, "2.0.1", "ECLAIR_0_1"},
    {7, "2.1", "ECLAIR_MR1"},
    {8, "2.2", "FROYO"},
    {9, "2.3.0", "GINGERBREAD"},
    {10, "2.3.3", "GINGERBREAD_MR1"},
    {11, "3.0", "HONEYCOMB"},
    {12, "3.1", "HONEYCOMB_MR1"},
    {13, "3.2", "HONEYCOMB_MR2"},
    {14, "4.0.1", "ICE_CREAM_SANDWICH"},
    {15, "4.0.3", "ICE_CREAM_SANDWICH_MR1"},
    {16, "4.1", "JELLY_BEAN"},
    {17, "4.2", "JELLY_BEAN_MR1"},
    {18, "4.3", "JELLY_BEAN_MR2"},
    {19, "4.4", "KITKAT"},
    {20, "4.4W", "KITKAT_WATCH"},
    {21, "5.0", "LOLLIPOP"},
    {22, "5.1", "LOLLIPOP_MR1"},
    {23, "6.0", "M"},
    {24, "7.0", "N"},
    {25, "7.1", "N_MR1"},
    {26, "8.0", "O"},
    {27, "8.1", "O_MR1"},
    {28, "9", "P"},
    {29, "10", "Q"},
    {30, "11", "R"},
    {31, "12", "S"},
    {32, "12L", "S_V2"},
    {33, "13", "TIRAMISU"},
    {34, "14", "UPSIDE_DOWN_CAKE"},
    {35, "15", "VANILLA_ICE_CREAM"},
    {36, "16", "BAKLAVA"},
    {37, "17", "CINNAMON_BUN"},
};

static const IG::Session::Device::AndroidVersion& ANDROID_FindByApi(int val) {
    auto it = std::ranges::find_if(ANDROID_API_MAP,
                                   [&](const IG::Session::Device::AndroidVersion& v) {
                                       return v.apiVersion == val;
                                   });

    if (it == ANDROID_API_MAP.end())
        throw AndroidApiTranslationError(val);

    return *it;
}

std::string ANDROID_TranslateApiVersionToCodebase(int val) {
    return ANDROID_FindByApi(val).codebaseVersion;
}

std::string ANDROID_TranslateApiVersionToStrVer(int val) {
    return ANDROID_FindByApi(val).niceVersion;
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
        } else if (errno == ENOENT)
            throw std::runtime_error(std::format("Unable to prepare classes.dex at {} (file was not populated)", classesPath));
        else
            throw std::runtime_error(std::format("Unable to prepare classes.dex: {}", std::strerror(errno)));
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
    const std::string& Device::KEYINFO_ANDROID_VERSION = "android_version";
    const std::string& Device::KEYINFO_ANDROID_CODEBASE = "android_codebase";
    const std::string& Device::KEYINFO_DISPLAY_DPI = "display_dpi";
    const std::string& Device::KEYINFO_DISPLAY_WIDTH = "display_width";
    const std::string& Device::KEYINFO_DISPLAY_HEIGHT = "display_height";
    const std::string& Device::KEYINFO_DEVICE_PRODUCT = "product";
    const std::string& Device::KEYINFO_DEVICE_MANUFACTURER = "manufacturer";
    const std::string& Device::KEYINFO_DEVICE_CODENAME = "codename";
    const std::string& Device::KEYINFO_DEVICE_CPU_LABEL = "cpu_label";

    const std::string& Device::KEYINFO_BROWSER_VERSION = "browser_version";

    std::string Device::MakeUserAgent() {
        // Example Android UA: "25/7.1.1; 440dpi; 1080x1920; Xiaomi; MI MAX 2; oxygen; qcom"
        std::stringstream ua;

        if (m_deviceType == Type::MOBILE) {
            ua << m_androidInfo.androidVersion.apiVersion << "/";
            ua << m_androidInfo.androidVersion.niceVersion << "; ";
            ua << m_androidInfo.displaySize.dpi << "dpi; ";
            ua << m_androidInfo.displaySize.width << "x" << m_androidInfo.displaySize.height << "; ";
            ua << m_androidInfo.manufacturer << "; ";
            ua << m_androidInfo.product << "; ";
            ua << m_androidInfo.codename << "; ";
            ua << m_androidInfo.cpuLabel;
        } else {
            auto browserVer = std::any_cast<std::string>(GetInfo(KEYINFO_BROWSER_VERSION));
            ua << "Mozilla/5.0 (";

            if (m_os == OperatingSystem::LINUX)
                ua << "X11; Linux x86_64;";
            else if (m_os == OperatingSystem::MAC_OS)
                ua << "Macintosh; Intel Mac OS X 10_15_7;";
            else if (m_os == OperatingSystem::WINDOWS)
                ua << "Windows NT 10.0; Win64; x64;";

            ua << " rv:" << browserVer << ") Gecko/20100101 Firefox/" << browserVer;
        }

#if __has_include(<OsintgramCXX/App/AppProps.hpp>)
        try {
            if (std::any_cast<bool>(GetInfo("IncludeOsintgramInformationAlongsideUserAgent"))) {
                ua << " Osintgram(" << OsintgramCXX_PlatformBuild << "; ";
                ua << OsintgramCXX_VersionName << "/" << OsintgramCXX_VersionCode << ")";
            }
        } catch (...) {
        }
#endif

        return ua.str();
    }

    Device::Type Device::GetDeviceType() {
        return m_deviceType;
    }

    void Device::SetDeviceType(const Type& type) {
        m_deviceType = type;
    }

    void Device::SetInfo(const std::string& key, const std::any& val) {
        deviceInfo[key] = val;

        if (m_deviceType == Type::MOBILE) {
            bool typeInt = val.type() == typeid(int);
            bool typeStr = val.type() == typeid(std::string);

            try {
                if (key == KEYINFO_ANDROID_API && typeInt) {
                    m_androidInfo.androidVersion.apiVersion = CastData<int>(val);
                    m_androidInfo.androidVersion.codebaseVersion = ANDROID_TranslateApiVersionToCodebase(
                        CastData<int>(val));
                    m_androidInfo.androidVersion.niceVersion = ANDROID_TranslateApiVersionToStrVer(CastData<int>(val));
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
                throw KeyHandlerError("Value type invalid (err = " + std::string(ex.what()) + ")");
            }
        }
    }

    std::any Device::GetInfo(const std::string& key) {
        if (m_deviceType == Type::MOBILE) {
            if (key == KEYINFO_ANDROID_API)
                return m_androidInfo.androidVersion.apiVersion;

            if (key == KEYINFO_ANDROID_VERSION)
                return m_androidInfo.androidVersion.niceVersion;

            if (key == KEYINFO_ANDROID_CODEBASE)
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

        throw KeyNotFoundError("Key " + key + " not found");
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
        std::string version = BROWSER_DEFAULT_VERSION;

#ifdef __linux__
        std::vector<std::string> firefoxAppInfoPaths = {
            "/usr/lib64/firefox/application.ini",
            "/usr/lib/firefox/application.ini",
            "/usr/share/firefox/application.ini",
            "/opt/firefox/application.ini"
        };
        std::string firefoxAppInfoPath;

        for (const auto& it : firefoxAppInfoPaths) {
            if (fs::exists(it)) {
                firefoxAppInfoPath = it;
                break;
            }
        }

        if (!firefoxAppInfoPath.empty()) {
            if (std::ifstream appInfoFile(firefoxAppInfoPath); appInfoFile.is_open()) {
                std::string line;
                bool inApp = false;
                while (std::getline(appInfoFile, line)) {
                    line = TrimString(line);
                    if (line.empty())
                        continue;

                    if (line == "[App]") {
                        inApp = true;
                        continue;
                    }

                    if (line.starts_with("Version=") && inApp) {
                        version = line.substr(8);
                        break;
                    }
                }
            }
        }
#elif defined(_WIN32) // #ifdef __linux__
        auto Win32Firefox_GetVersion = []() -> std::optional<std::string> {
            HKEY hKey = nullptr;
            DWORD cbData = 0;

            if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Mozilla\\Mozilla Firefox", 0, KEY_READ, &hKey) !=
                ERROR_SUCCESS) {
                if (RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Mozilla\\Mozilla Firefox", 0, KEY_READ, &hKey) !=
                    ERROR_SUCCESS) {
                    return std::nullopt;
                }
            }

            RegQueryValueExW(hKey, L"CurrentVersion", nullptr, nullptr, nullptr, &cbData);
            std::wstring wval(cbData / sizeof(wchar_t), L'\0');

            DWORD type = 0;
            if (RegQueryValueExW(hKey, L"CurrentVersion", nullptr, &type,
                reinterpret_cast<LPBYTE>(wval.data()), &cbData) != ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return std::nullopt;
            }

            RegCloseKey(hKey);

            while (!wval.empty() && wval.back() == L'\0')
                wval.pop_back();

            std::string result(wval.begin(), wval.end());
            return result;
        };

        if (std::optional<std::string> detectVer = Win32Firefox_GetVersion(); detectVer.has_value()) {
            version = detectVer.value();
            if (auto pos = version.find(' '); pos != std::string::npos)
                version.erase(pos);
        }
        // can't really leave a comment without CLion yapping about incorrect macro :sob:
#endif

        device.SetInfo(Device::KEYINFO_BROWSER_VERSION, version);
#endif

        return device;
    }
}
