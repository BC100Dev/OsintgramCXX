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
    fs::path result;

    // 1. look for libraries within the cwd (current working directory) and the executables directory
    if (fs::exists(OsintgramCXX::GetRootDirectory() + "/" + file))
        return OsintgramCXX::GetRootDirectory() + "/" + file;

    if (fs::exists(OsintgramCXX::CurrentWorkingDirectory() + "/" + file))
        return OsintgramCXX::CurrentWorkingDirectory() + "/" + file;

    std::vector<std::string> entryPaths;
#ifdef __linux__
    // 2. look for libraries in the "LD_LIBRARY_PATH" environment
    const char *ldLibPathEnv = getenv("LD_LIBRARY_PATH");
    if (ldLibPathEnv) {
        std::string ldEntry;

        while (std::getline(std::istringstream(ldLibPathEnv), ldEntry, ':')) {
            result = ldEntry / fs::path(file);

            if (fs::exists(result))
                return result.string();
        }
    }

    // 3. look for libraries in the system paths
    // as per tests suggest, this does take quite some time.
    // as long as the library exists within the executable's range, PWD and LD_LIBRARY_PATH, this shouldn't be a problem
    // otherwise, have fun
#if defined(__ANDROID__)
    std::string sysPaths = "/system/lib:/system/lib32:/system/lib64";
#else
    std::string sysPaths = "/usr/lib:/usr/lib32:/usr/lib64:/usr/local/lib:/usr/local/lib32:/usr/local/lib64:/lib:/lib32:/lib64";
#endif
    std::string entry;

    while (std::getline(std::stringstream(sysPaths), entry, ':')) {
        result = fs::path(entry) / fs::path(file);

        if (fs::exists(result))
            return result.string();
    }
#endif

#ifdef _WIN32
    // windows, at least you aren't this hard...
    // right?
    const char* pathEnv = getenv("PATH");
    if (pathEnv) {
        std::string pathEntry;

        while (std::getline(std::istringstream(pathEnv), pathEntry, ';')) {
            result = pathEntry / fs::path(file);

            if (fs::exists(result))
                return result.string();
        }
    }
#endif

    return "";
}

void *get_method_from_handle(void *handle, const char *symbol) {
#ifdef __linux__
    void *p = dlsym(handle, symbol);
    if (p == nullptr)
        throw std::runtime_error("dlsym failed: " + std::string(dlerror()));

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

            std::string objName;
            // construct platform name
#ifdef __linux__
            objName = "linux:";
#endif

#ifdef __ANDROID__
            objName = "android:";
#endif

#ifdef _WIN64
            objName = "win64:";
#endif

            // construct architecture
#ifdef __x86_64__
            objName.append("x64");
#endif

#ifdef __aarch64__
            objName.append("arm64");
#endif

            if (!command_set["lib"].contains(objName)) {
                std::cerr << "data for this current platform (" << objName << ") does not exist, passing on..." << std::endl;
                continue;
            }

            std::string libName = command_set["lib"][objName];
            std::string libPath = find_lib(libName);
            if (libPath.empty())
                throw std::runtime_error("Library " + libName + " not found");

#ifdef __linux__
            libHandle = dlopen(libPath.c_str(), RTLD_NOW);
#endif

#ifdef _WIN32
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

                        ModHandles::C_CommandExec cmdExec = [funcPtr](const char *_c, int a, char **b, int c,
                                                                      char **d) {
                            return reinterpret_cast<int (*)(const char *, int, char **, int, char **)>(funcPtr)(_c, a,
                                                                                                                b, c,
                                                                                                                d);
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