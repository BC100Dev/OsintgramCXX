#include <IGApi/Error.hpp>
#include <string>

namespace IG {
    ThrottledError::ThrottledError(long long timeout) : InstagramError(
        "You have been rate-limited for " +
        std::to_string(static_cast<double>(timeout) / 60) +
        " seconds") {
    }

    UserNotFoundError::UserNotFoundError(const std::string& user) : InstagramError(
        "User by the name of " + user + " not found") {
    }
}
