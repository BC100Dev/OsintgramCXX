## Security related things
# This library involves in certain critical events, such as:
# - User Authentication (needed in storing the Cookies)
# - File Encryption (securely store the User Authentication) (relying on OpenSSL through libcurl)

CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(OsintgramCXX-security STATIC ${ModSources})

# this eventually connects to the Networking, which connects to libcurl, which libcurl eventually connects to OpenSSL
# damn.
target_link_libraries(OsintgramCXX-security PRIVATE OsintgramCXX-networking ${OsintgramCXX_LINK_DEPS})