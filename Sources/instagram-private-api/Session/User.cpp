#include <IGApi/Session.hpp>

namespace IG::Session {

    User::User(const std::string& username) : m_activeDevice(Device::Type::MOBILE) {
    }
}
