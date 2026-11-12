## Security related things
# This library involves in certain critical events, such as:
# - User Authentication (needed in storing the Cookies)
# - File Encryption (securely store the User Authentication) (relying on OpenSSL)

CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(account-storage SHARED ${ModSources})

target_link_libraries(account-storage PRIVATE ${OsintgramCXX_LINK_OpenSSL} ${OsintgramCXX_LINK_DEPS})
SetRpathValue(account-storage)