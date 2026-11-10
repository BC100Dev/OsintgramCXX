#ifndef CREDENTIALS_INI_HPP
#define CREDENTIALS_INI_HPP

#include <optional>
#include <stdexcept>
#include <string>

namespace LegacyCreds {
    struct Credentials {
        std::string username;
        std::string password;
        std::string hikerApi;
    };

    class MalformedCredentialsFile : public std::runtime_error {
    public:
        explicit MalformedCredentialsFile(const std::string& msg) : std::runtime_error(msg) {
        }
    };

    bool HasIniFile();

    std::optional<Credentials> ParseIniFile();

    void InsertCredentials(const Credentials& creds);
}

#endif //CREDENTIALS_INI_HPP
