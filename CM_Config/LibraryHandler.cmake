find_package(CURL REQUIRED)
list(APPEND OsintgramCXX_LINK_DEPS CURL::libcurl)

if (APP_SYSTEM_TARGET STREQUAL "Windows")
    list(APPEND OsintgramCXX_LINK_DEPS crypt32 ws2_32 advapi32 secur32)
endif ()