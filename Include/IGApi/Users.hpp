#ifndef INSTAGRAM_USERS_HPP
#define INSTAGRAM_USERS_HPP

#include <vector>
#include <string>

#include "Session.hpp"

namespace IG::Users {
    /**
     * Finds the users that match the exact username given by the parameter.
     * @return a list of matching users that were provided by the string parameter. Limits the count of
     * users by a limit of 5 users.
     */
    std::vector<Session::Target> FindUsersByName(const std::string& username,
                                                       bool exactValue = true,
                                                       bool caseSensitive = false,
                                                       int max = 25);

    /**
     * Finds the target by the given user ID. It will either return the data assigned for the target,
     * or will return null, if the user is not found.
     */
    OptionalData<Session::Target> FindUserByID(const std::string& id);

    /**
     * Finds users by their display names. This will list all matching names by either the exact value (which might
     * return only one target), or will list even more users, if case sensitivity is not enabled. Such case is good,
     * when looking out for accounts that are impersonating someone else.
     */
    std::vector<Session::Target> FindUsersByDisplayName(const std::string& displayName,
                                                          bool exactValue = false,
                                                          bool caseSensitive = false,
                                                          int max = 25);
}

#endif //INSTAGRAM_USERS_HPP
