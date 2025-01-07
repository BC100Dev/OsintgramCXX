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

#include <nlohmann/json.hpp>
#include <curl/curl.h>

using namespace OsintgramCXX;
using namespace OsintgramCXX::Wine;
using namespace OsintgramCXX::Networking;
using namespace OsintgramCXX::Proxy;
using namespace nlohmann;

bool performNet() {
    RequestData reqData;
    reqData.readTimeoutMillis = 9000;
    reqData.connTimeoutMillis = 5000;
    reqData.url = "https://www.google.com";
    reqData.method = RequestMethod::CONNECT;
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

int main(int argc, char **argv) {
    curl_version_info_data *curlInfo = curl_version_info(CURLVERSION_NOW);
    std::cout << "SSL Backend: " << curlInfo->ssl_version << std::endl;

    std::cout << "User Domain: " << UserDomain() << std::endl;
    std::cout << "Hostname: " << GetHostname() << std::endl;
    std::cout << "Username: " << CurrentUsername() << std::endl;

    std::cout << "Performing network request..." << std::endl;
    return performNet() ? 0 : 1;
}