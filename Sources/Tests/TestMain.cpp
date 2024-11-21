// Test Main Executable (TestMain.cpp)
// "/test" directory excluded; any testing code goes in here

#include <iostream>
#include <format>

#include "../../Include/OsintgramCXX/Networking/Networking.hpp"
#include "../../Include/OsintgramCXX/Networking/Errors.hpp"

#include "../../Include/OsintgramCXX/Commons/HelpPage.hpp"
#include "../../Include/OsintgramCXX/Commons/Terminal.hpp"
#include "../../Include/OsintgramCXX/Commons/Utils.hpp"
#include "../../Include/OsintgramCXX/Commons/Tools.hpp"

#include "../../Include/OsintgramCXX/App/Shell/ShellEnv.hpp"
#include "../../Include/OsintgramCXX/App/Shell/Shell.hpp"
#include "../../Include/OsintgramCXX/App/AppProps.hpp"
#include "../../Include/OsintgramCXX/App/Defaults.hpp"
#include "../../Include/OsintgramCXX/App/Properties.hpp"
#include "../../Include/OsintgramCXX/App/WineDetect.hpp"

#include <OsintgramCXX_Libs/nlohmann/json.hpp>
#include <curl/curl.h>

using namespace OsintgramCXX;
using namespace OsintgramCXX::Wine;

int main(int argc, char **argv) {
    std::vector<char> ch = Pause("Press literally any key 3 times...\n", 3);
    for (const char& it : ch) {
        std::cout << "Pressed " << it << std::endl;
    }

    return 0;
}