cmake_minimum_required(VERSION 3.23)
project(OsintgramCXX)

set(CMAKE_CXX_STANDARD 23)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    message(STATUS "== Building on Android device")
    set(APP_TARGETS_ANDROID ON)
else ()
    set(APP_TARGETS_ANDROID OFF)
endif ()

if (NOT APP_TARGETS_ANDROID)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif ()

# available toolchains out of the box (see "toolchains" directory):
# Linux_amd64        = x86_64 / AMD64 / x64 Architectures
# Linux_arm64        = arm64 / aarch64 / arm-x64 Architectures (e.g. Raspberry Pi)
# Linux_RISCV64      = RISC-V 64-bit on Linux systems
# Linux_MinGW_Win64  = Windows MinGW x64 compilers on Linux
if (DEFINED APP_CONFIG_TARGET AND APP_CONFIG_TARGET)
    if (EXISTS "${CMAKE_SOURCE_DIR}/toolchains/${APP_CONFIG_TARGET}.cmake")
        include("${CMAKE_SOURCE_DIR}/toolchains/${APP_CONFIG_TARGET}.cmake")
    else ()
        message(FATAL_ERROR "Toolchain file '${APP_CONFIG_TARGET}.cmake' does not exist.\nMake sure that the Toolchain file exists.")
    endif ()
else ()
    if (UNIX AND NOT APPLE AND "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
        set(APP_SYSTEM_TARGET "Linux")
    elseif (WIN64)
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

if (DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE AND NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE is not configured to be a VCPKG CMake Configuration file.")
endif ()

set(CMAKE_SYSTEM_NAME ${APP_SYSTEM_TARGET})
set(CMAKE_SYSTEM_PROCESSOR ${APP_SYSTEM_ARCH})

## find libraries
find_package(CURL REQUIRED)

## Start project setup
file(GLOB_RECURSE APP_SOURCES "src/**/*.cpp" "src/**.cpp" "src/**/*.hpp" "src/**.hpp" "src/**/*.h" "src/**.h")
file(GLOB APP_INCLUDES "include/*")

set(APP_MAIN "${CMAKE_SOURCE_DIR}/src/MainExec.cpp")
list(REMOVE_ITEM APP_SOURCES ${APP_MAIN})

include_directories(${APP_INCLUDES})

set(OsintgramCXX_LINK_DEPS "CURL::libcurl")

if (${APP_SYSTEM_TARGET} STREQUAL "Windows")
    set(OsintgramCXX_LINK_DEPS ${OsintgramCXX_LINK_DEPS} crypt32 ws2_32)
endif ()

add_executable(OsintgramCXX ${APP_MAIN} ${APP_SOURCES})
target_link_libraries(OsintgramCXX PRIVATE ${OsintgramCXX_LINK_DEPS})

set_target_properties(OsintgramCXX PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

if (EXISTS "${CMAKE_SOURCE_DIR}/tests")
    file(GLOB_RECURSE TEST_SOURCES "tests/**/*.cpp" "tests/**.cpp" "tests/**/*.hpp" "tests/**.hpp" "tests/**/*.h" "tests/**.h")

    message(STATUS "OsintgramCXX: found testing code (adding to build instructions for target 'OsintgramCXX-testing')")
    set(OsintgramCXX_TESTING_CODE "found")

    add_executable(OsintgramCXX-testing ${APP_SOURCES} ${TEST_SOURCES})
    target_link_libraries(OsintgramCXX-testing PRIVATE ${OsintgramCXX_LINK_DEPS})
endif ()

if (APP_SYSTEM_TARGET STREQUAL "Linux")
    set_target_properties(OsintgramCXX PROPERTIES INSTALL_RPATH '$ORIGIN')
elseif (APP_SYSTEM_TARGET STREQUAL "Windows")
    # not sure, if we need advapi32 for this one
    set(OsintgramCXX_LINK_DEPS ${OsintgramCXX_LINK_DEPS} secur32 advapi32)

    target_link_libraries(OsintgramCXX PRIVATE ${OsintgramCXX_LINK_DEPS})

    if (OsintgramCXX_TESTING_CODE STREQUAL "found")
        target_link_libraries(OsintgramCXX-testing PRIVATE ${OsintgramCXX_LINK_DEPS})
    endif ()
endif ()