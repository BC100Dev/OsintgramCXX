#ifndef INSTAGRAM_ERROR_HPP
#define INSTAGRAM_ERROR_HPP

#include <stdexcept>
#include <string>

namespace IG {
    class InstagramError : public std::exception {
    public:
        explicit InstagramError(const std::string& msg) : m_message(std::move(msg)) {
        }

        [[nodiscard]] const char* what() const noexcept override {
            return m_message.c_str();
        }

    protected:
        std::string m_message;
    };

    class ThrottledError : public InstagramError {
    public:
        explicit ThrottledError(long long timeout);
    };

    class UserNotFoundError : public InstagramError {
    public:
        explicit UserNotFoundError(const std::string& user);
    };
}

#endif //INSTAGRAM_ERROR_HPP
