#ifndef OSINTGRAMCXX_ARCHIVAL_HPP
#define OSINTGRAMCXX_ARCHIVAL_HPP

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <optional>

namespace Testing::Archival {

    struct AccountData {
        // the username that can be typed into the browser's URL
        std::string name;

        // The display username that people see
        // an example is, where an account's username is all lowercase, but the display name has different letter cases.
        // for example, "bc100dev" is the username, and "BC100Dev" is the display name
        std::string displayName;

        // the account identifier that Instagram APIs give out
        // optional field, may not be registered, if the server APIs won't give the ID back in a rare event.
        std::optional<std::string> accountIdentifier;

        //
        bool hasProPic;
    };

    bool CreateAccountArchive(const std::string& name);

}

#endif //OSINTGRAMCXX_ARCHIVAL_HPP
