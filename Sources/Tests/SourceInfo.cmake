if (NOT DEFINED OsintgramCXX_CompileTestCode)
    return()
endif ()

if (NOT OsintgramCXX_CompileTestCode)
    message(STATUS "Ignore Testing Source Code")
    return()
endif ()

CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_executable(OsintgramCXX-testing ${ModSources})
if (APP_SYSTEM_TARGET STREQUAL "Linux")
    set_target_properties(OsintgramCXX-testing PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
    set_target_properties(OsintgramCXX-testing PROPERTIES INSTALL_RPATH '$ORIGIN:$ORIGIN/libs:$ORIGIN/lib:$ORIGIN/../lib')
endif ()

target_link_libraries(OsintgramCXX-testing PRIVATE ${OsintgramCXX_LINK_DEPS} OsintgramCXX-security dev-utils)