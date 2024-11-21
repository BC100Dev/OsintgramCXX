// Test Main Executable (TestMain.cpp)
// "/test" directory excluded; any testing code goes in here

#include <iostream>
#include <format>

#include <OsintgramCXX/Networking/Networking.hpp>
#include <OsintgramCXX/Networking/Errors.hpp>

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

bool performNet() {
    RequestData reqData;
    reqData.readTimeoutMillis = 9000;
    reqData.connTimeoutMillis = 5000;
    reqData.url = "https://bc100dev.net";
    reqData.method = RequestMethod::GET;
    reqData.version = HTTP_1_1;

    ResponseData respData = CreateRequest(reqData);
    if (!respData.errorData.empty()) {
        std::cout << "Error: " << respData.errorData << std::endl;
        return false;
    }

    std::cout << "Status: " << respData.statusCode << std::endl;
    return true;
}

int main(int argc, char **argv) {
    return performNet() ? 0 : 1;
}