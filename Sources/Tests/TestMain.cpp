#include <iostream>
#include <format>
#include <fstream>

#include <dev_tools/network/Network.hpp>
#include <dev_tools/network/URLParams.hpp>
#include <dev_tools/network/Errors.hpp>
#include <dev_tools/network/Proxy/Proxy.hpp>
#include <dev_tools/network/Proxy/ProxyChains.hpp>

#include <dev_tools/commons/HelpPage.hpp>
#include <dev_tools/commons/Terminal.hpp>
#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/commons/Tools.hpp>

#include <OsintgramCXX/App/Shell/ShellEnv.hpp>
#include <OsintgramCXX/App/Shell/Shell.hpp>
#include <OsintgramCXX/App/AppProps.hpp>
#include <OsintgramCXX/App/WineDetect.hpp>

#include <OsintgramCXX/Security/FileEncryption.hpp>

#include <nlohmann/json.hpp>
#include <curl/curl.h>

using namespace OsintgramCXX;
using namespace OsintgramCXX::Wine;
using namespace OsintgramCXX::FileEncryption;
using namespace DevTools;
using namespace nlohmann;

namespace fs = std::filesystem;

bool performNet() {
    RequestData reqData;
    reqData.readTimeoutMillis = 9000;
    reqData.connTimeoutMillis = 5000;
    reqData.url = "https://www.instagram.com";
    reqData.method = RequestMethod::REQ_GET;
    reqData.version = HTTP_1_1;

    ResponseData respData = CreateRequest(reqData);
    if (!respData.errorData.empty()) {
        std::cout << "Error: " << respData.errorData << std::endl;
        return false;
    }

    for (const auto& it : respData.headers) {
        std::cout << "Header \"" << it.first << "\" == \"" << it.second << "\"" << std::endl;
    }

    std::cout << "Status: " << respData.statusCode << std::endl;
    return true;
}

void encryptionDecryption() {
    if (!fs::exists("EncryptionTestingPhase.txt"))
        return;

    std::string pass = "super_secure_password";

    try {
        std::cout << "Attempt encryption..." << std::endl;
        EncryptionData encData = encrypt("EncryptionTestingPhase.txt", pass);
        std::cout << "Encryption passed" << std::endl;

        {
            std::ofstream off("Data.enc");
            if (off.is_open() && off.good()) {
                off.write(reinterpret_cast<const char*>(encData.uCharData.data()), encData.uCharData.size());
                off.close();
            }
            else {
                std::cerr << "Write unsuccessful" << std::endl;
                return;
            }
        }

        std::cout << "Encrypted data written\nAttempt decryption..." << std::endl;
        EncryptionData decData = decrypt("Data.enc", pass);
        std::cout << "Decryption passed" << std::endl;

        {
            std::ofstream off("Data.dec");
            if (off.is_open() && off.good()) {
                off.write(reinterpret_cast<const char*>(decData.uCharData.data()), decData.uCharData.size());
                off.close();
            }
            else {
                std::cerr << "Decryption write unsuccessful" << std::endl;
            }
        }

        std::cout << "Decrypted data written" << std::endl;
    }
    catch (const std::runtime_error& ex) {
        std::cerr << "Runtime Exception: " << ex.what() << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error" << std::endl;
    }
}

int main(int argc, char** argv) {
    /*
    encryptionDecryption();

    curl_version_info_data *curlInfo = curl_version_info(CURLVERSION_NOW);
    std::cout << "SSL Backend: " << curlInfo->ssl_version << std::endl;

    std::cout << "User Domain: " << UserDomain() << std::endl;
    std::cout << "Hostname: " << GetHostname() << std::endl;
    std::cout << "Username: " << CurrentUsername() << std::endl;

    std::cout << "Performing network request..." << std::endl;
    */

    std::vector<URLParam> euParams = {
        {"search", "hello world"},
        {"user", "contact@bc100dev.net"},
        {"filter", "active-user"},
        {"noval", std::nullopt},
        {"id", "123"},
    };
    std::string euUrl = EncodeURLParams(euParams, "https://api.example.com/data?existingdata=true", false);
    std::cout << "euUrl = " << euUrl << std::endl;

    for (std::vector<URLParam> duParams = DecodeURLParams(euUrl);
         const auto& [key, value] : duParams)
        std::cout << "Param " << key << (value.has_value() ? " => " + value.value() : "") << std::endl;

    return 0;
}
