#ifndef INSTAGRAM_NETWORK_HPP
#define INSTAGRAM_NETWORK_HPP

#include "Session.hpp"
#include "Users.hpp"

#include <optional>
#include <vector>

namespace IG {

    const std::string_view INSTAGRAM_BROWSER_;

    class Network {
    public:
        struct NetworkRequest {
            std::optional<Session::User> user;
            std::optional<Session::Target> target;
            Session::Device device;
            std::string url;
            std::vector<std::pair<std::string, std::string>> headers;
            std::optional<std::string> body;
        };

        struct NetworkResponse {
        };

        NetworkResponse net_get(const NetworkRequest &request);

        NetworkResponse net_post(const NetworkRequest &request);

        NetworkResponse net_put(const NetworkRequest &request);

        NetworkResponse net_patch(const NetworkRequest &request);

        NetworkResponse net_delete(const NetworkRequest &request);

    private:
        std::string m_urlPath;
    };

}

#endif //INSTAGRAM_NETWORK_HPP
