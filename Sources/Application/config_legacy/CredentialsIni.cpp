#include "CredentialsIni.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/commons/Paths.hpp>

namespace fs = std::filesystem;

namespace LegacyCreds {
    std::optional<fs::path> memFilePath = std::nullopt;

    static std::optional<fs::path> LocateIniFile() {
        static std::vector<fs::path> ConfigPaths = {
            ExecutableDirectory() / "config" / "credentials.ini",
            ExecutableDirectory() / "credentials.ini",
            App::Paths::UserConfig() / "OsintgramCXX" / "config" / "credentials.ini",
            App::Paths::UserConfig() / "OsintgramCXX" / "credentials.ini",
            CurrentWorkingDirectory() / "config" / "credentials.ini",
            CurrentWorkingDirectory() / ".config" / "credentials.ini"
        };

        for (const auto& it : ConfigPaths) {
            if (fs::exists(it) && fs::is_regular_file(it)) {
                if (!memFilePath.has_value())
                    memFilePath = it;

                return it;
            }
        }

        return std::nullopt;
    }

    bool HasIniFile() {
        std::optional<fs::path> iniFile = LocateIniFile();
        if (!iniFile.has_value())
            return false;

        return fs::exists(*iniFile) && fs::is_regular_file(*iniFile);
    }

    std::optional<Credentials> ParseIniFile() {
        if (!HasIniFile())
            return std::nullopt;

        if (!memFilePath.has_value()) {
            LocateIniFile();
            if (!memFilePath.has_value())
                return std::nullopt;
        }

        std::ifstream file(*memFilePath);
        if (!file.is_open())
            return std::nullopt;

        Credentials creds;
        bool inCredentialsSection = false;

        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);

            if (line.empty() || line[0] == ';' || line[0] == '#')
                continue;

            if (line[0] == '[') {
                const size_t closingBracket = line.find(']');
                if (closingBracket == std::string::npos)
                    throw MalformedCredentialsFile("malformed file: " + line);

                std::string sect = line.substr(1, closingBracket - 1);
                sect = TrimString(sect);
                inCredentialsSection = sect == "Credentials" || sect == "credentials";
                continue;
            }

            if (!inCredentialsSection)
                continue;

            const size_t eqPos = line.find('=');
            if (eqPos == std::string::npos)
                continue;

            const std::string key = TrimString(line.substr(0, eqPos));
            const std::string val = TrimString(line.substr(eqPos + 1));

            if (key == "username")
                creds.username = val;
            else if (key == "password")
                creds.password = val;
            else if (key == "hikerapi_token")
                creds.hikerApi = val;
        }

        file.close();
        return creds;
    }
}
