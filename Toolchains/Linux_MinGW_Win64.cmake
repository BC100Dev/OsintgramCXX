message("__ Using Toolchain 'Linux_MinGW_Win64' __")

message(STATUS "Setting C compiler to /usr/bin/x86_64-w64-mingw32-gcc")
set(CMAKE_C_COMPILER "/usr/bin/x86_64-w64-mingw32-gcc")

message(STATUS "Setting C++ compiler to /usr/bin/x86_64-w64-mingw32-g++")
set(CMAKE_CXX_COMPILER "/usr/bin/x86_64-w64-mingw32-g++")

set(APP_SYSTEM_TARGET "Windows")
set(APP_SYSTEM_ARCH "x86_64")

set(CMAKE_SYSTEM_NAME "Windows")