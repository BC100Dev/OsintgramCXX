message("__ Using Toolchain 'Linux_arm64' __")

message(STATUS "Setting C compiler to /usr/bin/aarch64-linux-gnu-gcc")
set(CMAKE_C_COMPILER "/usr/bin/aarch64-linux-gnu-gcc")

message(STATUS "Setting C++ compiler to /usr/bin/aarch64-linux-gnu-g++")
set(CMAKE_CXX_COMPILER "/usr/bin/aarch64-linux-gnu-g++")

set(APP_SYSTEM_TARGET "Linux")
set(APP_SYSTEM_ARCH "aarch64")

include_directories("${Z_VCPKG_ROOT_DIR}/installed/arm64-linux/include")