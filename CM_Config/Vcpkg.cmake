if (DEFINED SoftwareIgnoreVCPKG AND SoftwareIgnoreVCPKG EQUAL ON)
    return()
endif ()

if (DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE AND NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE is not configured to be a VCPKG CMake Configuration file.")
endif ()

if (DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    if (NOT DEFINED VCPKG_TARGET_TRIPLET)
        message(FATAL_ERROR "VCPKG_TARGET_TRIPLET is not defined")
    endif ()

    message(STATUS "VCPKG is enabled")
    include_directories("${Z_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/include")
    link_directories("${Z_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/lib")
endif ()