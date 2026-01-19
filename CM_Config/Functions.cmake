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
        return()
    endif ()

    target_link_options(${TARGET_NAME} PRIVATE "-Wl,--allow-shlib-undefined")
endfunction()

function(SetTargetOutputDir TARGET DIR)
    set_target_properties(${TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${DIR}"
            ARCHIVE_OUTPUT_DIRECTORY "${DIR}"
    )
endfunction()

function(SetRpathValue TARGET)
    set(RPATH_VAL "$ORIGIN:$ORIGIN/libs:$ORIGIN/lib:$ORIGIN/../lib:$ORIGIN/../libs")
    set_target_properties(${TARGET} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "${RPATH_VAL}"
            BUILD_RPATH "${RPATH_VAL}"
            INSTALL_RPATH_USE_LINK_PATH FALSE)
endfunction()