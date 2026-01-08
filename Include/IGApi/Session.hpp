#ifndef INSTAGRAM_SESSION_HPP
#define INSTAGRAM_SESSION_HPP

#include <vector>
#include <string>
#include <optional>
#include <map>
#include <any>
#include <iostream>

#include <nlohmann/json.hpp>
#include <dev_utils/network/Network.hpp>

using json = nlohmann::json;
using namespace DevUtils;

#define AuthenticationHeaders std::vector<std::pair<std::string, std::string>>
#define OptionalData std::optional
#define CastData std::any_cast

namespace IG::Session {

    /**
     * Represents an authenticated Instagram user.
     * This object holds user-specific data, such as the username and authentication headers (cookies, tokens).
     * Handles authentication flow, header provisioning, and ties into session context.
     */
    class User {
    public:
        /**
         * Constructs a user object with the given username, and if the user already exists on the system,
         * it will use the cached data from the filesystem itself. This does NOT perform login by itself.
         *
         * @param name The Instagram username to associate with this session
         */
        User(const std::string &name);

        /**
         * Retrieves the username associated with this session.
         *
         * @return String - stored Instagram username
         */
        std::string GetUsername() { return _userName; }

        /**
         * Attempts a login sequence for the user using the provided password.
         * This should initiate an authentication flow using the provided device information and credentials.
         * It is expected to internally populate _authHeaders upon success.
         *
         * @param pw Raw pointer to the password string to use for login
         * @throws AuthenticationError when an Authentication Request fails due to an unexpected Instagram APU response
         */
        void Login(const char *pw);

        /**
         * Retrieves the active authentication headers for this user session.
         * This typically includes cookies like sessionId, csrfToken and other required headers. These are passed
         * with every request to represent the logged-in user.
         *
         * @return a list of key-value pairs used in HTTP headers
         */
        AuthenticationHeaders GetAuthenticationHeaders();

        /**
         * This method goes with the key-value pairs of <code>GetAuthenticationHeaders</code> and verifies them with
         * the Instagram APIs that checks, whether the session is still authenticated.
         *
         * @return a true or false value, depending on authentication
         */
        bool IsAuthenticated();

    private:
        std::string _userName;
        AuthenticationHeaders _authHeaders;
    };

    /**
     * Represents a target account that ends up being monitored, captured and/or tracked.
     * Instead of relying on a user class that represents the Authentication flow, this class
     * represents the target that the user want to snoop around with, which includes the target-specific
     * metadata, queue history, profiling data and other things for OSINT use cases.
     */
    class Target {
    public:
    private:
    };

    /**
     * Abstract base class for all devices used in authentication and request construction. Devices
     * are expected to spoof data through a way called DSI, short for "Device Spoofing Information", something
     * that Instagram expects a legitimate device, but ends up being spoofed for the OSINT cases. This includes the
     * User-Agent, device model, OS version, DPI etc.
     */
    class Device {
    public:
        enum Type {
            MOBILE = 0,
            DESKTOP = 1,
        };

        virtual ~Device() = default;

        /**
         * Constructs a User-Agent string based on device properties.
         * Must be overridden to reflect the proper format required by the Instagram's APIs.
         * @return a callable User-Agent header value
         */
        virtual std::string MakeUserAgent() = 0;

        /**
         * Returns the type of device that ends up being spoofed.
         * @return The type itself, being either DESKTOP or MOBILE.
         */
        virtual Type GetDeviceType() = 0;

        /**
         * Injects a key-value pair into the device spoofing info map.
         * The key-value pair can effectively be anything, but the corresponding classes
         * of <code>MobileDevice</code> and <code>BrowserDevice</code> will require specific
         * key-value pairs.
         */
        virtual void SetInfo(const std::string &key, const std::any &val) = 0;

        /**
         * Retrieves a value from the device information map for the given key.
         * @param key The key that accesses the device map itself
         * @return Returns any type of value given by the SetInfo call.
         */
        virtual std::any GetInfo(const std::string &key) = 0;

    protected:
        std::map<std::string, std::any> deviceInfo;
    };

    /**
     * Specialized device class for mobile clients. This class will be commonly used throughout the API calls,
     * especially with the implementation of the device information fields used by Instagram's Android API.
     * All parameters are spoofed and used to construct a mobile information. An example may contain a Xiaomi device.
     */
    class MobileDevice : public Device {
    public:
        std::string MakeUserAgent() override;

        Type GetDeviceType() override {
            return MOBILE;
        }

        /**
         * Sets a value in the device information map, and updates the internal state required for the User Agent.
         * Handles recognized keys, such as:
         *
         * <ul>
         * <li>android_api (int)</li>
         * <li>android_version (string)</li>
         * <li>display_dpi (int|string)</li>
         * <li>width, height (int)</li>
         * <li>manufacturer, product, codename, cpu_label, instagram_app_version (string)</li>
         * </ul>
         *
         * @param key The needed key to insert information
         * @param val The needed value for the keymap value
         * @throws if casting fails or type mismatch occurs.
         */
        void SetInfo(const std::string &key, const std::any &val) override {
            deviceInfo[key] = val;

            bool typeInt = val.type() == typeid(int);
            bool typeStr = val.type() == typeid(std::string);

            try {
                if (key == "android_api" && typeInt) {
                    androidApi = CastData<int>(val);
                } else if (key == "android_version" && typeStr) {
                    androidVersion = CastData<std::string>(val);
                } else if (key == "display_dpi") {
                    if (typeInt)
                        displayDPI = std::to_string(CastData<int>(val)) + "dpi";
                    else if (typeStr)
                        displayDPI = CastData<std::string>(val);
                } else if ((key == "display_width" || key == "width") && typeInt) {
                    displaySize.width = CastData<int>(val);
                } else if ((key == "display_height" || key == "height") && typeInt) {
                    displaySize.height = CastData<int>(val);
                } else if ((key == "device_manufacturer" || key == "manufacturer") && typeStr) {
                    manufacturer = CastData<std::string>(val);
                } else if ((key == "device_product" || key == "product") && typeStr) {
                    product = CastData<std::string>(val);
                } else if ((key == "device_codename" || key == "codename") && typeStr) {
                    codename = CastData<std::string>(val);
                } else if ((key == "device_cpu_label" || key == "cpu_label") && typeStr) {
                    cpuLabel = CastData<std::string>(val);
                } else if (key == "instagram_app_version" && typeStr) {
                    instagramAppVersion = CastData<std::string>(val);
                }
            } catch (const std::bad_any_cast &ex) {
                throw std::runtime_error("Value type invalid");
            }
        }

        /**
         * Returns the value from the device information map. Exchanges specific keys for specific values.
         * @param key Supports the same keys as <code>SetInfo</code>
         * @return the value that is registered in the map. May return empty or 0, when not given a value.
         */
        std::any GetInfo(const std::string &key) override {
            if (key == "android_api") {
                return androidApi;
            } else if (key == "android_version") {
                return androidVersion;
            } else if (key == "display_dpi") {
                return displayDPI;
            } else if (key == "display_width" || key == "width") {
                return displaySize.width;
            } else if (key == "display_height" || key == "height") {
                return displaySize.height;
            } else if (key == "device_manufacturer" || key == "manufacturer") {
                return manufacturer;
            } else if (key == "device_product" || key == "product") {
                return product;
            } else if (key == "device_codename" || key == "codename") {
                return codename;
            } else if (key == "device_cpu_label" || key == "cpu_label") {
                return cpuLabel;
            } else if (key == "instagram_app_version") {
                return instagramAppVersion;
            }

            return deviceInfo[key];
        }

    private:
        struct DisplaySize {
            int width;
            int height;
        };

        int androidApi;
        std::string androidVersion;
        std::string displayDPI;
        DisplaySize displaySize;
        std::string manufacturer;
        std::string product;
        std::string codename;
        std::string cpuLabel;
        std::string instagramAppVersion;
    };

    /**
     * Represents a browser-based desktop client. It is effectively the same as the Mobile Device Representation,
     * but this class represents the desktop-based clients.
     */
    class BrowserDevice : public Device {
    public:
        enum OperatingSystem {
            LINUX = 0,
            WINDOWS = 1,
            MAC_OS = 2
        };

        std::string MakeUserAgent() override;

        Type GetDeviceType() override {
            return DESKTOP;
        }
    };

    /**
     * Retrieves the users that are currently connected
     */
    std::vector<User> GetConnectedUsers();

    /**
     * Retrieves the list of users that have been saved on the disk for offline / cache use.
     */
    std::vector<User> GetSavedUsers();

    /**
     * Retrieves the currently-active user. This can either return a non-value data, if there is currently no active
     * users connected, or when there is no active session on the user.
     */
    OptionalData<User> GetCurrentUserSession();

    /**
     * Retrieves the list of currently-assigned targets
     */
    std::vector<Target> GetTargetSessions();

    /**
     * Retrieves the list of saved targets on the disk. This can be helpful for offline / cache use, when there is no
     * active connection.
     */
    std::vector<Target> GetSavedTargets();

    /**
     * Retrieves the currently-assigned Target that is being operated on. May return null, if there is no Target
     * assigned with <code>sessionctl</code>, or when the list of currently-assigned targets is empty.
     * @return
     */
    OptionalData<Target> GetCurrentTargetSession();

    namespace Capture {

        /**
         * Finds the users that match the exact username given by the parameter.
         * @return a list of matching users that were provided by the string parameter. Limits the count of
         * users by a limit of 5 users.
         */
        std::vector<Target> FindTargetsByUsername(const std::string &username,
                                                  bool exactValue = true,
                                                  int max = 5);

        /**
         * Finds the target by the given user ID. It will either return the data assigned for the target,
         * or will return null, if the user is not found.
         */
        OptionalData<Target> FindTargetByID(const std::string &id);

        /**
         * Finds users by their display names. This will list all matching names by either the exact value (which might
         * return only one target), or will list even more users, if case sensitivity is not enabled. Such case is good,
         * when looking out for accounts that are impersonating someone else.
         */
        std::vector<Target> FindTargetsByDisplayName(const std::string &displayName,
                                                     bool exactValue = false,
                                                     bool caseSensitive = false,
                                                     int max = 100);

    }

    namespace Authentication {

        class AuthenticationError : public NetworkError {
        public:
            explicit AuthenticationError(const std::string &what) : NetworkError(what) {}
        };

        /**
         * A type of a 2FA, including the possibility of fetching the 2FA code through the means of
         * SMS (insecure), application (secure, e.g. Google Authenticator), or through a messaging app like WhatsApp.
         * That is, if people are still even using WhatsApp in 2025 .-.
         */
        enum TwoWayMethod {
            SMS = 0xA0,
            APP = 0xB0,
            MESSAGING_APP = 0xC0
        };

        /**
         * A certain returning type of the 2FA request. This will be validated by the Instagram APIs themselves,
         * and will either return a successful response, or one of the given errors.
         */
        enum TwoFA_ReturnType {
            SUCCESS = 0,
            INVALID_CODE = 1,
            TIMEOUT = 2,
            NETWORK_ERROR = 3,
            UNAUTHORIZED = 4,
            OTHER = 5
        };

        struct TwoFA_Validation {
            TwoFA_ReturnType retType = OTHER;
            ResponseData responseData{};
            User authenticatedUser;
        };

        /**
         * Verifies by either the Authorization Headers given by the user object or the Network Response Data, whether
         * a 2FA request is required or not.
         */
        bool Require2FA(User& user, ResponseData& responseData);

        /**
         * Creates a request to the Instagram API to retrieve the ability of sending (or retrieving) the 2FA code.
         * Optionally, a preferred method can be passed onto the request to have a specific way of fetching the 2FA
         * code. Might resort to an alternative, if the preferred method is not enabled on Instagram.
         */
        TwoWayMethod FetchAuthorizationMethod(const User& user, const OptionalData<TwoWayMethod>& preferredMethod);

        /**
         * Validates the 2FA code by checking the method against the Instagram API servers. This will return a
         * validation statement, whether a 2FA request has been successful or not. With the use of the initial
         * and the code sending network requests, this will allow the user object to be authenticated successfully.
         * Optionally, additional requesting headers can be added to the request. The user object passed as a parameter
         * here will be used for the Authorization Headers that are requested for the finalization of authorizing the
         * user object. Once the authentication is successful, the User object can be retrieved with
         * <code>GetConnectedUsers()</code>, and the user will automatically be put as an active user for interaction.
         */
        TwoFA_Validation Validate2FA(const std::string &code, const TwoWayMethod &method,
                                     const ResponseData &initialResponseData,
                                     const ResponseData &sendCodeData, const User &user,
                                     const Headers &requestHeaders);
    }

}

#endif //INSTAGRAM_SESSION_HPP
