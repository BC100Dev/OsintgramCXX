CollectSources(${PROJECT_MODULE_ROOT} ModSources)

set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
set(RPATH_VAL "$ORIGIN:$ORIGIN/libs:$ORIGIN/lib:$ORIGIN/../lib:$ORIGIN/../libs")

add_executable(OsintgramCXX ${ModSources})
SetTargetOutputDir(OsintgramCXX "${OUTPUT_DIRECTORY_ROOT}")
target_link_libraries(OsintgramCXX PRIVATE ${OsintgramCXX_LINK_DEPS} OsintgramCXX-commons OsintgramCXX-networking OsintgramCXX-security app-logger)
DisableBionicFortify(OsintgramCXX)

if (APP_SYSTEM_TARGET STREQUAL "Linux")
    if (NOT APP_TARGETS_ANDROID)
        target_link_libraries(OsintgramCXX PRIVATE cap)
    endif ()

    set_target_properties(OsintgramCXX PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "${RPATH_VAL}"
            BUILD_RPATH "${RPATH_VAL}"
            INSTALL_RPATH_USE_LINK_PATH FALSE)
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