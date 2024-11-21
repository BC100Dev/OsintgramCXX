message("__ Using Toolchain 'Linux_amd64' __")

message(STATUS "Setting C compiler to /usr/bin/x86_64-linux-gnu-gcc")
set(CMAKE_C_COMPILER "/usr/bin/x86_64-linux-gnu-gcc")

message(STATUS "Setting C++ compiler to /usr/bin/x86_64-linux-gnu-g++")
set(CMAKE_CXX_COMPILER "/usr/bin/x86_64-linux-gnu-g++")

set(APP_SYSTEM_TARGET "Linux")
set(APP_SYSTEM_ARCH "x86_64")

include_directories("${Z_VCPKG_ROOT_DIR}/installed/x64-linux/include")