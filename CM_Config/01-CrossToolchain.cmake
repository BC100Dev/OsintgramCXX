# Android Handler
if (CMAKE_SYSTEM_NAME STREQUAL "Android" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Target build is Android")
    set(APP_TARGETS_ANDROID ON)
endif ()

# Global Cross-Toolchains
if (DEFINED APP_CONFIG_TARGET AND APP_CONFIG_TARGET)
    if (EXISTS "${CMAKE_SOURCE_DIR}/Toolchains/${APP_CONFIG_TARGET}.cmake")
        include("${CMAKE_SOURCE_DIR}/Toolchains/${APP_CONFIG_TARGET}.cmake")
    else ()
        message(FATAL_ERROR "Toolchain file '${APP_CONFIG_TARGET}.cmake' does not exist.\nMake sure that the Toolchain file exists.")
    endif ()
else ()
    if (UNIX AND NOT APPLE AND "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
        set(APP_SYSTEM_TARGET "Linux")
    elseif (WIN64 OR MINGW)
        set(APP_SYSTEM_TARGET "Windows")
    elseif (UNIX AND APP_TARGETS_ANDROID)
        set(APP_SYSTEM_TARGET "Android")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
        set(APP_SYSTEM_ARCH "x86_64")
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(APP_SYSTEM_ARCH "aarch64")
    else ()
        message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif ()
endif ()