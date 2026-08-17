#include <IGApi/Data/UserInfo.hpp>
#include <IGApi/Error.hpp>

#include <dev_tools/commons/Utils.hpp>

using namespace DevTools;

namespace IG {

    // {"detail":"We're sorry, we couldn't find that.","exc_type":"UserNotFound"}

    UserInfo ParseJson2UserInfo(const json& data) {
        UserInfo info;

        RequireJsonKeys(data, {"user"}, "Instagram Error");
        const json& jUser = data["user"];
        RequireJsonKeys(jUser, {
                            "full_name",
                            "username",
                            "id",
                            "account_type",
                            "biography",
                            "hd_profile_pic_url_info",

                            "is_business",
                            "is_onboarding_account",
                            "is_private",
                            "is_verified",
                            "is_ring_creator",
                            "is_new_to_instagram",
                            "is_whatsapp_linked",

                            "category",
                            "city_name",
                            "public_email",
                            "public_phone_country_code",
                            "public_phone_number",
                            "address_street",
                            "external_url",

                            "has_private_collections",
                            "is_parenting_account"
                        }, "Instagram Error");

        const json& hdProfPic = jUser["hd_profile_pic_url_info"];
        RequireJsonKeys(hdProfPic, {
                            "height",
                            "width",
                            "url"
                        }, "Instagram Error");

        AccountType accType;
        switch (int iAccType = jUser["acc_type"].get<int>()) {
        case 1:
            accType = AccountType::PERSONAL;
            break;
        case 2:
            accType = AccountType::BUSINESS;
            break;
        case 3:
            accType = AccountType::CREATOR;
            break;
        default:
            throw std::runtime_error("Invalid account detected with identifier " + std::to_string(iAccType));
        }

        info.accountType = accType;
        info.fullName = jUser["full_name"].get<std::string>();
        info.username = jUser["username"].get<std::string>();
        info.userId = jUser["id"].get<std::string>();
        info.biography = jUser["biography"].get<std::string>();

        info.profPicUrl = hdProfPic["url"].get<std::string>();
        info.profPicHeight = hdProfPic["height"].get<int>();
        info.profPicWidth = hdProfPic["width"].get<int>();

        info.uBusiness = jUser["is_business"].get<bool>();
        info.uOnboardingAccount = jUser["is_onboarding_account"].get<bool>();
        info.uPrivate = jUser["is_private"].get<bool>();
        info.uVerified = jUser["is_verified"].get<bool>();
        info.uRingCreator = jUser["is_ring_creator"].get<bool>();
        info.uAnonymousProfilePicture = jUser["has_anonymous_profile_picture"].get<bool>();
        info.uNewAccount = jUser["is_new_to_instagram"].get<bool>();
        info.uWhatsappLinked = jUser["is_whatsapp_linked"].get<bool>();

        info.category = jUser["category"].get<std::string>();
        info.cityName = jUser["city_name"].get<std::string>();
        info.publicEmail = jUser["public_email"].get<std::string>();
        info.publicPhoneCountryCode = jUser["public_phone_country_code"].get<std::string>();
        info.publicPhoneNumber = jUser["public_phone_number"].get<std::string>();
        info.addressStreet = jUser["address_street"].get<std::string>();
        info.externalUrl = jUser["external_url"].get<std::string>();

        info.containsPrivateCollections = jUser["has_private_collections"].get<bool>();
        info.parentingAccount = jUser["is_parenting_account"].get<bool>();

        return info;
    }
}
