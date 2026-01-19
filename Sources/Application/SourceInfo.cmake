CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_executable(OsintgramCXX ${ModSources})
SetTargetOutputDir(OsintgramCXX "${OUTPUT_DIRECTORY_ROOT}")
target_link_libraries(OsintgramCXX PRIVATE ${OsintgramCXX_LINK_DEPS} devtools app-logger OsintgramCXX-security)

if (APP_SYSTEM_TARGET STREQUAL "Linux")
    if (NOT APP_TARGETS_ANDROID)
        target_link_libraries(OsintgramCXX PRIVATE cap)
    endif ()

    SetRpathValue(OsintgramCXX)
endif ()

# copy commands.json over
add_custom_command(TARGET OsintgramCXX POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/commands.json"
        "${OUTPUT_DIRECTORY_RESOURCES}/commands.json"
        COMMENT "Copying commands.json to build directory"
)

# copy AppSettings.cfg over
add_custom_command(TARGET OsintgramCXX POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/AppSettings.cfg"
        "${OUTPUT_DIRECTORY_RESOURCES}/AppSettings.cfg"
        COMMENT "Copying AppSettings.cfg to build directory"
)

# DO NOT REMOVE THIS
# Android begs you
DisableBionicFortify(OsintgramCXX)