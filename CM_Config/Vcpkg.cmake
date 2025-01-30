if (DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE AND NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE is not configured to be a VCPKG CMake Configuration file.")
endif ()

#set(CMAKE_SYSTEM_NAME ${APP_SYSTEM_TARGET})
#set(CMAKE_SYSTEM_PROCESSOR ${APP_SYSTEM_ARCH})

include_directories("${Z_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/include")
link_directories("${Z_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/lib")