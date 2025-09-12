CollectSources(${PROJECT_MODULE_ROOT} ModSources)

set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
set(RPATH_VAL "$ORIGIN:$ORIGIN/libs:$ORIGIN/lib:$ORIGIN/../lib:$ORIGIN/../libs")

add_executable(OsintgramCXX ${ModSources})
target_link_libraries(OsintgramCXX PRIVATE ${OsintgramCXX_LINK_DEPS} OsintgramCXX-commons OsintgramCXX-networking OsintgramCXX-security)

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