if (NOT APP_TARGETS_ANDROID)
    message(STATUS "Skipping module Android_Java")
    return()
endif ()

if (APP_SYSTEM_TARGET STREQUAL "Android" AND DEFINED ENV{TERMUX_VERSION})
    message(WARNING "The module [OsintgramCXX-AndroidParts] that will automatically resolve the display information will not be copied due to lack of Android SDK being built for Android itself. It is strongly recommended to build it on a Desktop device. Alternatively, you can rent out a server.")
    return()
endif ()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_custom_target(OsintgramCXX-AndroidParts ALL
        COMMAND ${CMAKE_COMMAND} -E env
                CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}
                ${Python3_EXECUTABLE} "${PROJECT_MODULE_ROOT}/compile.py"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Compiling Java sources to DEX"
        VERBATIM)