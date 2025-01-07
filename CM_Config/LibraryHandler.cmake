# we need the OpenSSL port. we need it.
# otherwise, the "OsintgramCXX-security" module fails.
# I don't want to co-link against OpenSSL, when curl already has it as a dependency
set(CURL_USE_OPENSSL ON)
set(CURL_USE_SCHANNEL OFF)

find_package(CURL REQUIRED)
list(APPEND OsintgramCXX_LINK_DEPS CURL::libcurl)

if (APP_SYSTEM_TARGET STREQUAL "Windows")
    list(APPEND OsintgramCXX_LINK_DEPS crypt32 ws2_32 advapi32 secur32)
endif ()