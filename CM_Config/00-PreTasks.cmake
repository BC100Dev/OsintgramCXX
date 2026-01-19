if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
else ()
    set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/libs:$ORIGIN/lib")
    set(CMAKE_BUILD_RPATH "$ORIGIN:$ORIGIN/libs:$ORIGIN/lib")
endif ()

set(OsintgramCXX_LINK_DEPS "")

if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif ()

set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)