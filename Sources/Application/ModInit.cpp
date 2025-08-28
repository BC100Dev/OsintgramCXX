#include "ModInit.hpp"

#include <OsintgramCXX/App/Shell/ShellEnv.hpp>

#include <OsintgramCXX/App/ModHandles.hpp>
#include <OsintgramCXX/Commons/Utils.hpp>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

#ifdef __linux__

#include <dlfcn.h>

#endif

#ifdef _WIN32

#include <windows.h>

#endif

using json = nlohmann::json;

namespace fs = std::filesystem;

std::string currentProcessingLibrary;

#ifdef __linux__
const char *c_home = std::getenv("HOME");
#endif

#ifdef _WIN32
const char *c_userProfile = std::getenv("USERPROFILE");
#endif

std::vector<std::string> lookupPaths = {
        OsintgramCXX::CurrentWorkingDirectory() + "/Resources",
        OsintgramCXX::CurrentWorkingDirectory(),
        OsintgramCXX::ExecutableDirectory() + "/Resources",
        OsintgramCXX::ExecutableDirectory(),

#ifdef __linux__
        "/etc/OsintgramCXX",
        std::string(c_home ? c_home : "/home/" + OsintgramCXX::CurrentUsername()) + "/.config/OsintgramCXX",
        "/Data/base/" + std::to_string(getuid()) + "/OsintgramCXX",
        "/Data/base/shared/OsintgramCXX",
#endif

#ifdef _WIN32
        std::string(c_userProfile ? c_userProfile : R"(C:\Users\)" + OsintgramCXX::CurrentUsername()) +
        R"(\AppData\Local\OsintgramCXX)"
#endif
};

fs::path locate_json(const std::string &filename);

std::string find_lib(const std::string &file) {
    fs::path result;

    // 1. look for libraries within the cwd (current working directory) and the executables directory
    if (fs::exists(OsintgramCXX::ExecutableDirectory() + "/" + file))
        return OsintgramCXX::ExecutableDirectory() + "/" + file;

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
    std::string sysPaths = "/data/data/com.termux/files/usr/lib:/system/lib:/system/lib32:/system/lib64";
#else
    // yes, I added that AnlinxOS path, deal with it :skull:
    std::string sysPaths =
            "/usr/lib:/usr/lib32:/usr/lib64:/usr/local/lib:/usr/local/lib32:/usr/local/lib64:/lib:/lib32:/lib64:/System/" +
            std::string(CPU_ARCHITECTURE);
#endif

    std::string entry;
    std::stringstream ss(sysPaths);

    while (std::getline(ss, entry, ':')) {
        result = fs::path(entry) / fs::path(file);

        if (fs::exists(result))
            return result.string();
    }

    // 4. Persistent / User Storage paths
    std::vector<std::string> storagePaths = {
            std::string(c_home ? c_home : "/home/" + OsintgramCXX::CurrentUsername()) + "/.config/OsintgramCXX/mods.d",
            "/etc/OsintgramCXX/mods.d",
            "/etc/OsintgramCXX/mods.d/" + std::string(CPU_ARCHITECTURE),
            "/usr/share/OsintgramCXX/mods.d",
            "/usr/share/OsintgramCXX/mods.d/" + std::string(CPU_ARCHITECTURE),
            "/Data/base/" + std::to_string(getuid()) + "/OsintgramCXX/mods.d/" + std::string(CPU_ARCHITECTURE),
            "/Data/base/shared/OsintgramCXX/mods.d/" + std::string(CPU_ARCHITECTURE)
    };
    for (const auto &it: storagePaths) {
        if (fs::exists(it))
            return it;
    }
#endif

#ifdef _WIN32
    // windows, at least you aren't this hard...
    // right?
    const char *pathEnv = getenv("PATH");
    if (pathEnv) {
        std::string pathEntry;
        std::istringstream ss(pathEnv);

        while (std::getline(ss, pathEntry, ';')) {
            result = pathEntry / fs::path(file);

            if (fs::exists(result))
                return result.string();
        }
    }

    if (c_userProfile) {
        std::string userProfile = std::string(c_userProfile);
        std::vector<std::string> winPaths = {
                userProfile + R"(\AppData\Local\OsintgramCXX)",
                userProfile + R"(\AppData\Local\OsintgramCXX\mods.d)",
                userProfile + R"(\AppData\Local\OsintgramCXX\mods.d\)" + std::string(CPU_ARCHITECTURE)
        };

        for (const auto& it : winPaths) {
            result = fs::path(it) / file;
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
        throw std::runtime_error("dlsym failed for " + currentProcessingLibrary + ": " + std::string(dlerror()));

    return p;
#endif

#ifdef _WIN32
    return (void *) GetProcAddress((HMODULE) handle, symbol);
#else
    // for you macOS users :skull:
    return nullptr;
#endif
}

std::string get_error_from_lib() {
#ifdef __linux__
    return dlerror();
#elif defined(_WIN32)
    char buf[512] = {0};
    unsigned long len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                       nullptr, GetLastError(), 0, buf, sizeof(buf), nullptr);
    if (len == 0)
        return "[UNKNOWN_ERROR]";

    buf[len] = '\0';
    return buf;
#else
    return "[UNSPECIFIED]";
#endif
}

void parse_json(const json &j) {
    if (!j.is_object() || !j.contains("command_sets"))
        throw std::runtime_error("Unexpected JSON content");

    if (j["command_sets"].is_array() && !j["command_sets"].empty()) {
        for (const auto &command_set: j["command_sets"]) {
            if (command_set.is_string() && !std::string(command_set).empty()) {
                fs::path jsonFile = locate_json(command_set);
                if (fs::exists(jsonFile)) {
                    std::string data;

                    // we use scope block to make the file closing happen as soon as the scope block is finished
                    {
                        std::ifstream in(jsonFile);
                        if (in.is_open()) {
                            std::stringstream ss;
                            ss << in.rdbuf();

                            data = ss.str();
                        }
                    }

                    parse_json(json::parse(data));
                }

                return;
            }

            void *libHandle = nullptr;
            if (!command_set.contains("label") && !command_set["label"].is_string())
                throw std::runtime_error("invalid command set (\"label\" key is missing or contains invalid data)");

            if (!command_set.contains("id") && !command_set["id"].is_number())
                throw std::runtime_error("invalid command set (\"id\" key is missing or contains invalid data)");

            OsintgramCXX::LibraryEntry libEntryData{};
            libEntryData.label = command_set["label"];
            libEntryData.id = command_set["id"];

            if (command_set.contains("author") && command_set["author"].is_string())
                libEntryData.author = command_set["author"];

            if (command_set.contains("description") && command_set["description"].is_string())
                libEntryData.description = command_set["description"];

            std::string objName;
            // construct platform name
#ifdef __linux__
            objName = "linux:";
#endif

#ifdef __ANDROID__
            objName = "android:";
#endif

#ifdef _WIN64
            objName = "windows:";
#endif

            // construct architecture
#ifdef __x86_64__
            objName.append("x64");
#endif

#ifdef __aarch64__
            objName.append("arm64");
#endif

            if (!command_set["lib"].contains(objName)) {
                std::cerr << "data for this current platform (" << objName << ") does not exist, passing on..."
                          << std::endl;
                continue;
            }

            if (!command_set["lib"][objName].is_string())
                throw std::runtime_error("library entry for \"" + objName + "\" is not a string");

            std::string libName = command_set["lib"][objName];
            std::string libPath = find_lib(libName);
            if (libPath.empty())
                throw std::runtime_error("Library " + libName + " not found");

            currentProcessingLibrary = libName;

#ifdef __linux__
            libHandle = dlopen(libPath.c_str(), RTLD_NOW);
#endif

#ifdef _WIN32
            libHandle = LoadLibraryA(libPath.c_str());
#endif

            if (command_set["handlers"].is_object() && !command_set["handlers"].empty()) {
                json h_obj = command_set["handlers"];
                std::string symbolName;

                if (h_obj.contains("OnLoad") && h_obj["OnLoad"].is_string()) {
                    symbolName = h_obj["OnLoad"];

                    libEntryData.handler_onLoad = [libHandle, libName, symbolName]() -> int {
                        using FunctionType = int();
                        void *funcPtr = get_method_from_handle(libHandle, symbolName.c_str());
                        if (!funcPtr) {
                            std::cerr << "[ERROR] Failed to resolve symbol from \"" << libName << "\": " << symbolName
                                      << " -> "
                                      << get_error_from_lib() << std::endl;
                            return -1;
                        }

                        return reinterpret_cast<FunctionType *>(funcPtr)();
                    };
                }

                if (h_obj.contains("OnStop") && h_obj["OnStop"].is_string()) {
                    symbolName = h_obj["OnStop"];

                    libEntryData.handler_onExit = [libHandle, libName, symbolName]() -> int {
                        using FunctionType = int();
                        void *funcPtr = get_method_from_handle(libHandle, symbolName.c_str());
                        if (!funcPtr) {
                            std::cerr << "[ERROR] Failed to resolve symbol from " << libName << ": " << symbolName
                                      << " -> "
                                      << get_error_from_lib() << std::endl;
                            return -1;
                        }

                        return reinterpret_cast<FunctionType *>(funcPtr)();
                    };
                }

                if (h_obj.contains("OnCommandExec") && h_obj["OnCommandExec"].is_string()) {
                    symbolName = h_obj["OnCommandExec"];

                    libEntryData.handler_onCmdExec = [libHandle, libName, symbolName](char *cmdLine) {
                        using FunctionType = void(char *);
                        void *funcPtr = get_method_from_handle(libHandle, symbolName.c_str());
                        if (!funcPtr) {
                            std::cerr << "[ERROR] Failed to resolve symbol from " << libName << ": " << symbolName
                                      << " -> "
                                      << get_error_from_lib() << std::endl;
                            return;
                        }

                        reinterpret_cast<FunctionType *>(funcPtr)(cmdLine);
                    };
                }
            }

            if (command_set["cmd_list"].is_array() && !command_set["cmd_list"].empty()) {
                for (const auto &cmd: command_set["cmd_list"]) {
                    if ((cmd.contains("cmd") && cmd.contains("description") && cmd.contains("exec_symbol")) &&
                        (cmd["cmd"].is_string() && cmd["description"].is_string() && cmd["exec_symbol"].is_string())) {
                        std::string cmdName = cmd["cmd"];
                        std::string desc = cmd["description"];
                        std::string sym = cmd["exec_symbol"];

                        void *funcPtr = get_method_from_handle(libHandle, sym.c_str());
                        if (funcPtr == nullptr)
                            throw std::runtime_error(
                                    std::string("Command symbol for ").append(libName) + " not found, " + sym);

                        OsintgramCXX::C_CommandExec cmdExec = [funcPtr](const char *_c, int a, char **b, int c,
                                                                        char **d) {
                            return reinterpret_cast<int (*)(const char *, int, char **, int, char **)>(funcPtr)(_c, a,
                                                                                                                b, c,
                                                                                                                d);
                        };

                        OsintgramCXX::ShellLibEntry cmdEntry{};
                        cmdEntry.cmd = cmdName;
                        cmdEntry.description = desc;
                        cmdEntry.execHandler = cmdExec;

                        libEntryData.commands.push_back(cmdEntry);
                    }
                }
            }

            OsintgramCXX::loadedLibraries[libHandle] = libEntryData;
        }
    }
}

fs::path locate_json(const std::string &filename) {
    for (const auto &it: lookupPaths) {
        if (fs::exists(fs::path(it) / filename))
            return fs::path(fs::path(it) / filename);
    }

    return "";
}

void init_data() {
    std::vector<std::string> jsonFiles;

    const char *cJsonFile = getenv("OsintgramCXX_JsonCommandList");
    if (cJsonFile) {
        std::string _e = cJsonFile;
        std::string delim;

#ifdef _WIN32
        delim = ";";
#else
        delim = ":";
#endif

        if (OsintgramCXX::StringContains(_e, delim)) {
            jsonFiles = OsintgramCXX::SplitString(_e, delim);
        }
    }

    for (const auto &it: lookupPaths) {
        std::string path = it + "/commands.json";
        if (fs::exists(path))
            jsonFiles.emplace_back(path);
    }

    if (jsonFiles.empty()) {
        std::cerr << "No files under the name of commands.json found." << std::endl;
        return;
    }

    for (const auto &it: jsonFiles) {
        json j;
        std::ifstream in(it);
        if (!in.is_open()) {
            std::cerr << "Could not open command list file \"" << it << "\", continuing..." << std::endl;
            continue;
        }

        in >> j;
        parse_json(j);
    }
}

void ModLoader_load() {
    init_data();
}

void ModLoader_start() {
    // this method just calls on the handlers of "OnLoad", threaded.
    for (const auto &[unused, vec]: OsintgramCXX::loadedLibraries) {
        if (vec.handler_onLoad != nullptr)
            vec.handler_onLoad();
    }
}

void ModLoader_stop() {
    for (const auto &[handle, vec]: OsintgramCXX::loadedLibraries) {
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

namespace OsintgramCXX {
    std::map<void *, LibraryEntry> loadedLibraries;
}