set(CURL_USE_OPENSSL ON)
set(CURL_USE_SCHANNEL OFF)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

list(APPEND OsintgramCXX_LINK_DEPS CURL::libcurl)
list(APPEND OsintgramCXX_LINK_OpenSSL OpenSSL::SSL OpenSSL::Crypto)

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

    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        set(PTHREADS_WIN64_ENABLED OFF)
        if (NOT DEFINED PTHREADS_WIN64_MINGW_DIR)
            set(PTHREADS_WIN64_MINGW_DIR "/usr/x86_64-w64-mingw32")
        endif ()

        if (EXISTS "${PTHREADS_WIN64_MINGW_DIR}/bin/libwinpthread-1.dll"
                AND EXISTS "${PTHREADS_WIN64_MINGW_DIR}/lib/libwinpthread.a")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
        endif ()
    endif ()
endif ()

if (APP_SYSTEM_TARGET STREQUAL "Windows")
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
    set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
endif()