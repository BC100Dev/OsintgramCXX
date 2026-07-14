#include <iostream>
#include <fstream>
#include <csignal>

#include <dev_tools/commons/Dialog.hpp>
#include <dev_tools/commons/Terminal.hpp>
#include <dev_tools/commons/Utils.hpp>

#include <dev_tools/network/Network.hpp>

#include <OsintgramCXX/App/Shell/ShellEnv.hpp>
#include <OsintgramCXX/App/WineDetect.hpp>

#include <OsintgramCXX/Security/FileEncryption.hpp>

#include <nlohmann/json.hpp>

using namespace OsintgramCXX;
using namespace OsintgramCXX::Wine;
using namespace OsintgramCXX::FileEncryption;
using namespace DevTools;
using namespace nlohmann;

namespace fs = std::filesystem;

void exceptionHandler() {
    if (std::exception_ptr exPtr = std::current_exception()) {
        try {
            std::rethrow_exception(exPtr);
        } catch (const std::exception& ex) {
            std::ostringstream sw;
            sw << ex.what();

            Terminal::println(std::cerr, Terminal::TermColor::RED,
                              "The application has unexpectedly crashed. Cause of this error:", false);
            Terminal::println(std::cerr, Terminal::TermColor::RED, sw.str(), true);
        }
    } else
        Terminal::println(std::cerr, Terminal::TermColor::RED, "Unknown Termination captured", true);

    std::raise(SIGSEGV);
}

int main(int argc, char** argv) {
    std::set_terminate(exceptionHandler);

    /*
    === ERRORS ===
    Status: 401
    Body: {"state":false,"error":"Unauthorized request: pass access_key or login via https://hikerapi.com/login"}

    === SUCCESS ===
    Status: 200
    Body: {"requests":100,"rate":1,"currency":"USD","amount":2.0}
    */

    const std::string token = "YOUR_TOKEN_HERE";

    RequestData req;
    req.method = RequestMethod::REQ_GET;
    req.url = "https://api.hikerapi.com/sys/balance";
    req.version = HttpVersion::HTTP_1_1;
    req.headers.emplace_back("x-access-key", token);
    req.headers.emplace_back("accept", "application/json");

    ResponseData res = CreateRequest(req);

    std::cout << "Status: " << res.statusCode << std::endl;
    std::cout << "Body: " << ReadByteData(res.body) << std::endl;

    return 0;
}
