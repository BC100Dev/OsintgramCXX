#include "ModInit.hpp"

#include <OsintgramCXX/App/Defaults.hpp>
#include <OsintgramCXX/App/Shell/Shell.hpp>
#include <OsintgramCXX/App/Shell/ShellEnv.hpp>

#include <OsintgramCXX/App/ModHandles.hpp>
#include <OsintgramCXX/Commons/Utils.hpp>

#include <nlohmann/json.hpp>

#include <filesystem>

#ifdef __linux__

#include <dlfcn.h>

#endif

#ifdef _WIN32
#include <windows.h>
#endif

using json = nlohmann::json;

namespace fs = std::filesystem;

std::string find_lib(const std::string &file) {
    std::string result;

    if (fs::exists(OsintgramCXX::GetRootDirectory() + "/" + file))
        return OsintgramCXX::GetRootDirectory() + "/" + file;

    if (fs::exists(OsintgramCXX::CurrentWorkingDirectory() + "/" + file))
        return OsintgramCXX::CurrentWorkingDirectory() + "/" + file;

    std::vector<std::string> entryPaths;
#ifdef __linux__
    // 1. look for libraries in the "LD_LIBRARY_PATH" environment
    const char *ldLibPathEnv = getenv("LD_LIBRARY_PATH");
    if (ldLibPathEnv) {
        std::string ldEntry;

        while (std::getline(std::istringstream(ldLibPathEnv), ldEntry, ':')) {
            result = ldEntry;
            result.append("/").append(file);

            if (fs::exists(result))
                return result;
        }
    }

    // 2. look for libraries in the system paths
    std::string sysPaths = "/usr/lib:/usr/lib32:/usr/lib64:/usr/local/lib:/usr/local/lib32:/usr/local/lib64:/lib:/lib32:/lib64";
    std::string entry;

    while (std::getline(std::stringstream(sysPaths), entry, ':')) {
        result = entry;
        result.append("/").append(file);

        if (fs::exists(result))
            return result;
    }

    // 3. look for libraries in the user paths
    const char *homeEnv = getenv("HOME");
    if (homeEnv) {
        std::ostringstream oss;
        oss << homeEnv << "/.local/lib:" << homeEnv << "/.local/lib32:" << homeEnv << "/.local/lib64:" << homeEnv
            << "/lib:" << homeEnv << "/lib32:" << homeEnv << "/lib64:" << homeEnv << "/.lib:" << homeEnv
            << "/.lib32:" << homeEnv << "/.lib64";

        while (std::getline(std::stringstream(oss.str()), entry, ':')) {
            result = entry;
            result.append("/").append(file);

            if (fs::exists(result))
                return result;
        }
    }
#endif

#ifdef _WIN32
    // windows, at least you aren't this hard...
    // right?
    const char* pathEnv = getenv("PATH");
    if (pathEnv) {
        std::string pathEntry;

        while (std::getline(std::istringstream(pathEnv), pathEntry, ';')) {
            result = pathEntry;
            result.append("\\");
            result.append(file);

            if (fs::exists(result))
                return result;
        }
    }
#endif

    return "";
}

void *get_method_from_handle(void *handle, const char *symbol) {
#ifdef __linux__
    void *p = dlsym(handle, symbol);
    if (p == nullptr) {
        std::ostringstream oss;
        oss << "dlsym failed: " << dlerror();
        throw std::runtime_error(oss.str());
    }

    return p;
#endif

#ifdef _WIN32
    return (void*) GetProcAddress((HMODULE) handle, symbol);
#else
    // for you macOS users :skull:
    return nullptr;
#endif
}

std::string get_error_from_extlib() {
#ifdef __linux__
    return dlerror();
#endif

#ifdef _WIN32
    char buf[512] = {0};
    int len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, GetLastError(), 0, buf, sizeof(buf), NULL);
    if (len == 0)
        return "[UNKNOWN_ERROR]";

    buf[len] = '\0';
    return buf;
#endif

    return "[UNSPECIFIED]";
}

void parse_json(const json &j) {
    if (j["command_sets"].is_array() && !j["command_sets"].empty()) {
        for (const auto &command_set: j["command_sets"]) {
            void *libHandle = nullptr;

            ModHandles::LibraryEntry libEntryData{};

#ifdef __linux__
            std::string libName = command_set["lib"]["linux"];
            std::string libPath = find_lib(libName);
            if (libPath.empty())
                throw std::runtime_error("Library not found");

            libHandle = dlopen(libPath.c_str(), RTLD_NOW);
#endif

#ifdef _WIN32
            std::string libName = command_set["lib"]["win64"];
            std::string libPath = find_lib(libName);
            if (libPath.empty())
                throw std::runtime_error("Library not found");

            libHandle = LoadLibraryA(libPath.c_str());
#endif

            if (command_set["handlers"].is_array() && !command_set["handlers"].empty()) {
                for (const auto &handler: command_set["handlers"]) {
                    if (handler["handler"].is_string() && handler["symbol"].is_string()) {
                        std::string handlerName = handler["handler"];
                        std::string symbolName = handler["symbol"];

                        if (handlerName == "OnLoad") {
                            libEntryData.handler_onLoad = [libHandle, symbolName]() -> int {
                                using FunctionType = int();
                                void *funcPtr = get_method_from_handle(libHandle, symbolName.c_str());
                                if (!funcPtr) {
                                    std::cerr << "[ERROR] Failed to resolve symbol: " << symbolName << " -> "
                                              << get_error_from_extlib() << std::endl;
                                    return -1;
                                }

                                return reinterpret_cast<FunctionType *>(funcPtr)();
                            };
                        } else if (handlerName == "OnStop") {
                            libEntryData.handler_onExit = [libHandle, symbolName]() -> int {
                                using FunctionType = int();
                                void *funcPtr = get_method_from_handle(libHandle, symbolName.c_str());
                                if (!funcPtr) {
                                    std::cerr << "[ERROR] Failed to resolve symbol: " << symbolName << " -> "
                                              << get_error_from_extlib() << std::endl;
                                    return -1;
                                }

                                return reinterpret_cast<FunctionType *>(funcPtr)();
                            };
                        } else if (handlerName == "OnCommandExec") {
                            libEntryData.handler_onCmdExec = [libHandle, symbolName](char *cmdLine) {
                                using FunctionType = void(char *);
                                void *funcPtr = get_method_from_handle(libHandle, symbolName.c_str());
                                if (!funcPtr) {
                                    std::cerr << "[ERROR] Failed to resolve symbol: " << symbolName << " -> "
                                              << get_error_from_extlib() << std::endl;
                                    return;
                                }

                                reinterpret_cast<FunctionType *>(funcPtr)(cmdLine);
                            };
                        } else
                            throw std::runtime_error("Unknown handler type, " + handlerName);
                    }
                }
            }

            if (command_set["cmd_list"].is_array() && !command_set["cmd_list"].empty()) {
                for (const auto &cmd: command_set["cmd_list"]) {
                    if (cmd["cmd"].is_string() && cmd["description"].is_string() && cmd["exec_symbol"].is_string()) {
                        std::string cmdName = cmd["cmd"];
                        std::string desc = cmd["description"];
                        std::string sym = cmd["exec_symbol"];

                        void *funcPtr = get_method_from_handle(libHandle, sym.c_str());
                        if (funcPtr == nullptr)
                            throw std::runtime_error("Command symbol not found, " + sym);

                        ModHandles::C_CommandExec cmdExec = [funcPtr](int a, char **b, int c, char **d) {
                            return reinterpret_cast<int (*)(int, char **, int, char **)>(funcPtr)(a, b, c, d);
                        };

                        ModHandles::ShellLibEntry libEntry{};
                        OsintgramCXX::ShellCommand cmdEntry{};

                        libEntry.cmd = cmdName;
                        libEntry.description = desc;
                        libEntry.execHandler = cmdExec;

                        cmdEntry.commandName = cmdName;
                        cmdEntry.quickHelpStr = desc;
                        cmdEntry.libEntry = libEntry;

                        OsintgramCXX::ShellFuckery::add_command(cmdEntry);

                        libEntryData.commands.push_back(libEntry);
                    }
                }
            }

            ModHandles::loadedLibraries[libHandle] = libEntryData;
        }
    }
}

void init_data() {
    std::string jsonFile;

    const char *cJsonFile = getenv("OsintgramCXX_JsonCommandList");
    if (cJsonFile)
        jsonFile = cJsonFile;
    else
        jsonFile = OsintgramCXX::GetRootDirectory() + "/commands.json";

    if (!fs::exists(jsonFile)) {
        jsonFile = OsintgramCXX::GetRootDirectory() + "/Resources/commands.json";

        if (!fs::exists(jsonFile))
            throw std::runtime_error("Could not find commands.json");
    }

    json j;
    std::ifstream in(jsonFile);
    in >> j;

    parse_json(j);
}

void ModLoader_load() {
    init_data();
}

void ModLoader_start() {
    // this method just calls on the handlers of "OnLoad", threaded.
    for (const auto &[unused, vec]: ModHandles::loadedLibraries) {
        if (vec.handler_onLoad != nullptr)
            vec.handler_onLoad();
    }
}

void ModLoader_stop() {
    for (const auto &[handle, vec]: ModHandles::loadedLibraries) {
        if (vec.handler_onExit != nullptr)
            vec.handler_onExit();

#ifdef __linux__
        if (handle)
            dlclose(handle);
#endif

#ifdef _WIN32
        if (handle)
            FreeLibrary((HMODULE) handle);
#endif
    }
}

namespace ModHandles {
    std::map<void *, LibraryEntry> loadedLibraries;
}