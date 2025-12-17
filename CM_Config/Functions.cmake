function(CollectSources RootDir OutVar)
    set(SEARCH_PATTERNS
            "${RootDir}/*.cpp"
            "${RootDir}/*.hpp"
            "${RootDir}/*.c"
            "${RootDir}/*.h"
            "${RootDir}/*.cc"
            "${RootDir}/*.cuh"
            "${RootDir}/**/*.cpp"
            "${RootDir}/**/*.hpp"
            "${RootDir}/**/*.c"
            "${RootDir}/**/*.h"
            "${RootDir}/**/*.cc"
            "${RootDir}/**/*.cuh")

    file(GLOB_RECURSE SOURCE_FILES ${SEARCH_PATTERNS})
    set(${OutVar} ${SOURCE_FILES} PARENT_SCOPE)
endfunction()

function(DisableBionicFortify TARGET_NAME)
    if (NOT APP_TARGETS_ANDROID)
        message("!! DisableBionicFortify was called on a non-Android build configuration.")
        message("!! This message can be ignored, if you are building for Linux or Windows itself.")
        return()
    endif ()

    target_link_options(${TARGET_NAME} PRIVATE "-Wl,--allow-shlib-undefined")
endfunction()