#include <IGApi/Session.hpp>

namespace IG::Session {

    User::User(const std::string& username) : m_activeDevice(Device::Type::MOBILE) {
    }

    void User::Login(const char* pw) {
        std::cerr << "IG::Session::User#Login(pw) - stubbed" << std::endl;
    }
}
