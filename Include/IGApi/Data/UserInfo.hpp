#ifndef INSTAGRAM_USERINFO_HPP
#define INSTAGRAM_USERINFO_HPP

#include <string>

#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace IG {
    enum class AccountType : int {
        PERSONAL = 1,
        BUSINESS = 2,
        CREATOR = 3,
    };

    struct UserInfo {
        std::string fullName;
        std::string username;
        std::string biography;
        std::string userId;
        AccountType accountType;

        std::string profPicUrl; // key "hd_profile_pic_url_info" -> "url"
        int profPicWidth;
        int profPicHeight;

        bool uBusiness;
        bool uOnboardingAccount;
        bool uPrivate;
        bool uVerified;
        bool uRingCreator;
        bool uAnonymousProfilePicture;
        bool uNewAccount; // key "is_new_to_instagram"
        bool uWhatsappLinked;

        std::string category;
        std::string cityName;
        std::string publicEmail;
        std::string publicPhoneCountryCode;
        std::string publicPhoneNumber;
        std::string addressStreet;
        std::string externalUrl;

        bool containsPrivateCollections;
        bool parentingAccount;
    };

    inline std::string AccountTypeToStr(const AccountType& type) {
        switch (type) {
        case AccountType::PERSONAL:
            return "Personal";
        case AccountType::BUSINESS:
            return "Business";
        case AccountType::CREATOR:
            return "Creator";
        default:
            return "";
        }
    }

    UserInfo ParseJson2UserInfo(const json& data);
}

#endif //INSTAGRAM_USERINFO_HPP
