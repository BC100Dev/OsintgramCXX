#ifndef OSINTGRAMCXX_PROPERTIES_HPP
#define OSINTGRAMCXX_PROPERTIES_HPP

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <chrono>
#include <filesystem>
#include <cstdlib>

#include <OsintgramCXX/Commons/Utils.hpp>

namespace fs = std::filesystem;
using namespace OsintgramCXX;

namespace OsintgramCXX {
    class Properties {
    public:
        Properties() = default;

        ~Properties() {
            propMap.clear();
        }

        void LoadSettings() {
            std::string rootSettingsPath = ExecutableDirectory();
            std::string userEnvName;

#ifdef __linux__
            userEnvName = "HOME";
            rootSettingsPath.append("/share/SettingsData.cfg");
#elif _WIN32
            userEnvName = "USERPROFILE";
            rootSettingsPath.append(R"(Contents\SettingsData.cfg)");
#else
            throw std::runtime_error("Unsupported OS");
#endif

            LoadSettings(rootSettingsPath);

            const char *userEnv = std::getenv(userEnvName.c_str());
            if (userEnv != nullptr) {
                std::string sUserEnv(userEnv);
                if (!sUserEnv.empty()) {
                    fs::path userSettings; // = sUserEnv + R"(\AppData\Local\BC100Dev\OsintgramCXX\config\SettingsData.cfg)";

#ifdef __linux__
                    userSettings = sUserEnv + R"(/.config/net.bc100dev/OsintgramCXX/SettingsData.cfg)";
#elif _WIN32
                    userSettings = sUserEnv + R"(\AppData\Local\BC100Dev\OsintgramCXX\config\SettingsData.cfg)";
#endif

                    if (fs::exists(userSettings))
                        LoadSettings(userSettings.string());
                }
            }
        }

        void LoadSettings(const std::string &filePath) {
            std::fstream fileLoad(filePath);
            if (!fileLoad.is_open())
                throw std::runtime_error("File Open Error: " + filePath);

            std::string line;
            while (std::getline(fileLoad, line)) {
                line = TrimString(line);

                if (line.empty())
                    continue;

                if (line[0] == '#')
                    continue;

                if (line.find('=') == std::string::npos) {
                    propMap[line] = "";
                    continue;
                }

                std::stringstream ss(line);
                std::string key, value;

                if (std::getline(ss, key, '=') && std::getline(ss, value)) {
                    key = TrimString(key);
                    value = TrimString(value);
                    propMap[key] = value;
                }
            }

            fileLoad.close();
        }

        std::string GetProperty(const std::string &key) {
            return GetProperty(key, "");
        }

        std::string GetProperty(const std::string &key, const std::string &defValue) {
            auto it = propMap.find(key);
            return it != propMap.end() ? it->second : defValue;
        }

        std::vector<std::string> KeySet() {
            std::vector<std::string> keys;
            keys.reserve(propMap.size());

            for (const auto &i: propMap)
                keys.push_back(i.first);

            return keys;
        }

        std::vector<std::string> ValueSet() {
            std::vector<std::string> values;
            values.reserve(propMap.size());

            for (const auto &i: propMap)
                values.push_back(i.second);

            return values;
        }

        [[maybe_unused]] void SetProperty(const std::string &key, const std::string &value) {
            if (key.empty())
                return;

            if (key[0] == '#')
                throw std::runtime_error("SetPropertyError = key is a comment");

            propMap[key] = value;
        }

        [[maybe_unused]] void WriteToFile(const std::string &filePath) {
            std::ofstream stream(filePath);
            if (!stream.is_open())
                throw std::runtime_error("File Open Error (Properties.hpp#WriteToFile): \"" + filePath + "\"");

            stream << "## Properties.hpp" << std::endl;
            stream << "## codebase of OsintgramCXX" << std::endl;
            stream << "## Last updated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
                   << std::endl << std::endl;

            for (const auto &i: propMap) {
                if (i.first[0] == '#')
                    continue;

                stream << i.first << " = " << i.second << std::endl;
            }

            stream.close();
        }

    private:
        std::unordered_map<std::string, std::string> propMap;
    };
}

#endif //OSINTGRAMCXX_PROPERTIES_HPP
