if (DEFINED _LINUX_MINGW_WIN64_LOADED)
    return()
endif()
set(_LINUX_MINGW_WIN64_LOADED TRUE)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

message("__ Using Toolchain 'Linux_MinGW_Win64' __")

message(STATUS "Setting C compiler to /usr/bin/x86_64-w64-mingw32-gcc")
set(CMAKE_C_COMPILER "/usr/bin/x86_64-w64-mingw32-gcc")

message(STATUS "Setting C++ compiler to /usr/bin/x86_64-w64-mingw32-g++")
set(CMAKE_CXX_COMPILER "/usr/bin/x86_64-w64-mingw32-g++")

set(APP_SYSTEM_TARGET "Windows")
set(APP_SYSTEM_ARCH "x86_64")