#ifndef INSTAGRAM_USERS_HPP
#define INSTAGRAM_USERS_HPP

#include <string>

#include "Session.hpp"

namespace IG::Users {
    Session::Target FindUserByUsername(const std::string& username);

    Session::Target FindUserByUrl(const std::string& url);

    Session::Target FindUserById(const std::string& id);
}

#endif //INSTAGRAM_USERS_HPP
