CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_executable(OsintgramCXX ${ModSources})
target_link_libraries(OsintgramCXX PRIVATE ${OsintgramCXX_LINK_DEPS} OsintgramCXX-commons OsintgramCXX-networking OsintgramCXX-security)

if (APP_SYSTEM_TARGET STREQUAL "Linux")
    if (NOT APP_TARGETS_ANDROID)
        target_link_libraries(OsintgramCXX PRIVATE cap)
    endif ()

    set_target_properties(OsintgramCXX PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
    set_target_properties(OsintgramCXX PROPERTIES INSTALL_RPATH '$ORIGIN:$ORIGIN/libs:$ORIGIN/lib:$ORIGIN/../lib:$ORIGIN/../libs')
endif ()