set(CURL_USE_OPENSSL ON)
set(CURL_USE_SCHANNEL OFF)

## Encryption / Decryption relevant tasks (required by CURL for HTTPS communication)
find_package(OpenSSL REQUIRED)

## Required for Networking
find_package(CURL REQUIRED)

## Required for Local Account Storage (used by CURL)
find_package(ZLIB REQUIRED)

## Let's make it a requirement, especially for Windows users
find_package(Threads REQUIRED)

list(APPEND OsintgramCXX_LINK_DEPS CURL::libcurl ZLIB::ZLIB)
list(APPEND OsintgramCXX_LINK_OpenSSL OpenSSL::SSL OpenSSL::Crypto)

if (DEFINED APP_TARGETS_ANDROID AND NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
    list(APPEND OsintgramCXX_LINK_DEPS "-static-libstdc++")
endif ()

if (APP_SYSTEM_TARGET STREQUAL "Windows")
    list(APPEND OsintgramCXX_LINK_DEPS crypt32 ws2_32 advapi32 secur32)

    ## issues with MinGW on Arch: re-create the libraries for OpenSSL that it needs
    # why? stupid "-ldl" flag.
    # hot garbage, am I right or am I right
    if ("dl" IN_LIST OPENSSL_LIBRARIES)
        list(REMOVE_ITEM OPENSSL_LIBRARIES "dl")
    endif ()

    set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_LINK_LIBRARIES "crypt32;ws2_32")
    set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_LINK_LIBRARIES "crypt32;ws2_32")

    list(APPEND OsintgramCXX_LINK_DEPS "-Wl,-Bstatic,--whole-archive" "winpthread" "-Wl,--no-whole-archive")

    if (APP_SYSTEM_TARGET STREQUAL "Windows")
        set(CMAKE_SHARED_LIBRARY_PREFIX "")
        set(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
        set(CMAKE_STATIC_LIBRARY_PREFIX "")
        set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
    endif ()
endif ()