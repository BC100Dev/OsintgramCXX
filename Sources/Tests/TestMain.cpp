#include <iostream>
#include <format>

#include <OsintgramCXX/Networking/Networking.hpp>
#include <OsintgramCXX/Networking/Errors.hpp>
#include <OsintgramCXX/Networking/Proxy/Proxy.hpp>
#include <OsintgramCXX/Networking/Proxy/ProxyChains.hpp>

#include <OsintgramCXX/Commons/HelpPage.hpp>
#include <OsintgramCXX/Commons/Terminal.hpp>
#include <OsintgramCXX/Commons/Utils.hpp>
#include <OsintgramCXX/Commons/Tools.hpp>

#include <OsintgramCXX/App/Shell/ShellEnv.hpp>
#include <OsintgramCXX/App/Shell/Shell.hpp>
#include <OsintgramCXX/App/AppProps.hpp>
#include <OsintgramCXX/App/Defaults.hpp>
#include <OsintgramCXX/App/Properties.hpp>
#include <OsintgramCXX/App/WineDetect.hpp>

#include <OsintgramCXX/Security/FileEncryption.hpp>

#include <nlohmann/json.hpp>
#include <curl/curl.h>

using namespace OsintgramCXX;
using namespace OsintgramCXX::Wine;
using namespace OsintgramCXX::Networking;
using namespace OsintgramCXX::Proxy;
using namespace OsintgramCXX::FileEncryption;
using namespace nlohmann;

bool performNet() {
    RequestData reqData;
    reqData.readTimeoutMillis = 9000;
    reqData.connTimeoutMillis = 5000;
    reqData.url = "https://www.instagram.com";
    reqData.method = RequestMethod::GET;
    reqData.version = HTTP_1_1;

    ResponseData respData = CreateRequest(reqData);
    if (!respData.errorData.empty()) {
        std::cout << "Error: " << respData.errorData << std::endl;
        return false;
    }

    for (const auto &it: respData.headers) {
        std::cout << "Header \"" << it.first << "\" == \"" << it.second << "\"" << std::endl;
    }

    std::cout << "Status: " << respData.statusCode << std::endl;
    return true;
}

void encryptionDecryption() {
    std::string pass = "super_secure_password";

    try {
        std::cout << "Attempt encryption..." << std::endl;
        EncryptionData encData = OsintgramCXX::FileEncryption::encrypt("EncryptionTestingPhase.txt", pass);
        std::cout << "Encryption passed" << std::endl;

        std::ofstream off("Data.enc");
        if (off.is_open() && off.good()) {
            off.write(reinterpret_cast<const char *>(encData.uCharData.data()), encData.uCharData.size());
            off.close();
        } else {
            std::cerr << "Write unsuccessful" << std::endl;
            return;
        }

        std::cout << "Encrypted data written\nAttempt decryption..." << std::endl;
        EncryptionData decData = OsintgramCXX::FileEncryption::decrypt("Data.enc", pass);
        std::cout << "Decryption passed" << std::endl;

        std::ofstream off2("Data.dec");
        if (off2.is_open() && off2.good()) {
            off2.write(reinterpret_cast<const char *>(decData.uCharData.data()), decData.uCharData.size());
            off2.close();
        } else {
            std::cerr << "Decryption write unsuccessful" << std::endl;
        }

        std::cout << "Decrypted data written" << std::endl;
    } catch (const std::runtime_error &ex) {
        std::cerr << "Runtime Exception: " << ex.what() << std::endl;
    } catch (const std::exception &ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
    }
}

int main(int argc, char **argv) {
    encryptionDecryption();

    curl_version_info_data *curlInfo = curl_version_info(CURLVERSION_NOW);
    std::cout << "SSL Backend: " << curlInfo->ssl_version << std::endl;

    std::cout << "User Domain: " << UserDomain() << std::endl;
    std::cout << "Hostname: " << GetHostname() << std::endl;
    std::cout << "Username: " << CurrentUsername() << std::endl;

    std::cout << "Performing network request..." << std::endl;
    return performNet() ? 0 : 1;
}