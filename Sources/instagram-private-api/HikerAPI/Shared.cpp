#include <IGApi/HikerAPI.hpp>

namespace IG::Session {

    std::optional<HikerAPI> sharedHikerAPI;

    HikerAPI& GetSharedHikerInstance() {
        if (!sharedHikerAPI.has_value())
            throw std::runtime_error("Shared HikerAPI instance was not initialized");

        return *sharedHikerAPI;
    }

    HikerAPI InitializeSharedHikerInstance(const std::string& authToken) {
        return InitializeSharedHikerInstance(authToken, true);
    }

    HikerAPI InitializeSharedHikerInstance(const std::string& authToken, bool override) {
        if (sharedHikerAPI.has_value()) {
            if (override) {
                sharedHikerAPI = HikerAPI(authToken);
                return sharedHikerAPI.value();
            }

            throw HikerError("Shared HikerAPI instance is already present");
        }

        sharedHikerAPI = HikerAPI(authToken);
        return sharedHikerAPI.value();
    }

}