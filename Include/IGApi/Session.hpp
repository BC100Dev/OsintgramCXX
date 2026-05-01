#ifndef INSTAGRAM_SESSION_HPP
#define INSTAGRAM_SESSION_HPP

#include <vector>
#include <string>
#include <optional>
#include <map>
#include <any>
#include <iostream>

#include <nlohmann/json.hpp>
#include <dev_tools/network/Network.hpp>

#include "Session.hpp"

using json = nlohmann::json;
using namespace DevTools;

#define AuthenticationHeaders std::vector<std::pair<std::string, std::string>>
#define OptionalData std::optional
#define CastData std::any_cast

namespace IG::Session {

    class AuthenticationError : public NetworkError {
    public:
        explicit AuthenticationError(const std::string &what) : NetworkError(what) {}
    };

    /**
     * Base class for all devices used in authentication and request construction. Devices are expected to spoof data
     * through a way called DSI, short for "Device Spoofing Information", something that Instagram expects a legitimate
     * device, but ends up being spoofed for the OSINT cases. This includes the User-Agent, device model, OS version,
     * DPI etc.
     */
    class Device {
    public:
        struct DisplaySize {
            int width;
            int height;
            int dpi;
        };

        enum class Type {
            MOBILE = 0,
            DESKTOP = 1,
        };

        enum class OperatingSystem {
            WINDOWS = 0xF9,
            LINUX = 0xFE,
            MAC_OS = 0xD3,
            ANDROID = 0xAF,
            IOS = 0xD6,
            UNDEFINED = 0x00
        };

        struct AndroidVersion {
            int apiVersion;
            std::string codebaseVersion;
        };

        struct AndroidInfo {
            AndroidVersion androidVersion = {28, "9"};
            DisplaySize displaySize = {};
            std::string manufacturer;
            std::string product;
            std::string codename;
            std::string cpuLabel;
        };

        static const std::string& KEYINFO_ANDROID_API;
        static const std::string& KEYINFO_ANDROID_VERSION;
        static const std::string& KEYINFO_DISPLAY_DPI;
        static const std::string& KEYINFO_DISPLAY_WIDTH;
        static const std::string& KEYINFO_DISPLAY_HEIGHT;
        static const std::string& KEYINFO_DEVICE_PRODUCT;
        static const std::string& KEYINFO_DEVICE_MANUFACTURER;
        static const std::string& KEYINFO_DEVICE_CODENAME;
        static const std::string& KEYINFO_DEVICE_CPU_LABEL;

        ~Device() = default;

        explicit Device(const Type& type) : m_deviceType(type) {}
        explicit Device() : m_deviceType(Type::MOBILE) {}

        /**
         * Constructs a User-Agent string based on device properties.
         * Must be overridden to reflect the proper format required by the Instagram's APIs.
         * @return a callable User-Agent header value
         */
        std::string MakeUserAgent();

        /**
         * Returns the type of device that ends up being spoofed.
         * @return The type itself, being either DESKTOP or MOBILE.
         */
        Type GetDeviceType();

        /**
         * Injects a key-value pair into the device spoofing info map.
         * The key-value pair can effectively be anything, but the corresponding classes
         * of <code>MobileDevice</code> and <code>BrowserDevice</code> will require specific
         * key-value pairs.
         */
        void SetInfo(const std::string &key, const std::any &val);

        /**
         * Retrieves a value from the device information map for the given key.
         * @param key The key that accesses the device map itself
         * @return Returns any type of value given by the SetInfo call.
         */
        std::any GetInfo(const std::string &key);

        OperatingSystem GetOperatingSystem();

        void SetOperatingSystem(const OperatingSystem &os);

    protected:
        std::map<std::string, std::any> deviceInfo;
        OperatingSystem m_os = OperatingSystem::UNDEFINED;
        Type m_deviceType;
        AndroidInfo m_androidInfo;
    };

    Device CopyHostDeviceSetup();

    /**
     * Represents an authenticated Instagram user.
     * This object holds user-specific data, such as the username and authentication headers (cookies, tokens).
     * Handles authentication flow, header provisioning, and ties into session context.
     */
    class User {
    public:
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
        enum class TwoFA_ReturnType : int {
            SUCCESS = 0xF00,
            INVALID_CODE = 0xF01,
            TIMEOUT = 0xF02,
            NETWORK_ERROR = 0xF03,
            UNAUTHORIZED = 0xF04,
            OTHER = 0xFFF
        };

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

        std::vector<TwoWayMethod> GetTwoWayAuthenticationMethods();

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

        bool RequiresTwoFactorAuth();

        void AuthenticateTwoFactor(const std::string& twoFactorCode);

    private:
        std::string _userName;
        AuthenticationHeaders _authHeaders;
        Device m_activeDevice;
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

}

#endif //INSTAGRAM_SESSION_HPP
