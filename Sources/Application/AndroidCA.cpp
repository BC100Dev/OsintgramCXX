#include "AndroidCA.hpp"
#include <OsintgramCXX/App/Defaults.hpp>

#include <filesystem>

#ifdef __ANDROID__
bool env_exists() {
    const char* _ = std::getenv("OsintgramCXX_CertStore");
    return _ != nullptr;
}

std::vector<std::string> cert_store_paths = {
        env_exists() ? std::getenv("OsintgramCXX_CertStore") : "",
        OsintgramCXX::GetRootDirectory() + "/android_certstore.pem",
        "/data/data/com.termux/files/usr/etc/tls/cert.pem"
};

std::vector<std::string> sys_cert_store_paths = {
        "/system/etc/security/cacerts",
        "/system/etc/security/cacerts_google"
};

bool has_cert_store() {
    for (const auto& it : cert_store_paths) {
        if (it.empty())
            // looking at you, env variable!
            continue;

        if (fs::exists(it))
            return true;
    }

    return false;
}

void gen_cert_store(const std::string& path) {
    std::ofstream out(OsintgramCXX::GetRootDirectory() + "/android_certstore.pem", std::ios::out | std::ios::app);
    if (!out.is_open())
        throw std::runtime_error("Cannot open android_certstore.pem for writing");

    for (const auto& it : fs::directory_iterator(path)) {
        fs::path f_path = fs::absolute(it);
        std::ifstream fin(f_path.string(), std::ios::in);
        if (!fin.is_open()) {
            std::cerr << "Cannot open \"" << it << "\", skipping entry" << std::endl;
            continue;
        }

        out << fin.rdbuf();
        fin.close();
    }
}

void make_cert_store() {
    for (const auto& it : sys_cert_store_paths) {
        if (fs::exists(it))
            gen_cert_store(it);
    }
}
#endif

namespace OsintgramCXX::AndroidVer {

    void prepare_cacerts() {
#ifdef __ANDROID__
        std::string exec_path = OsintgramCXX::GetRootDirectory();

        if (has_cert_store())
            make_cert_store();
#endif
    }

}