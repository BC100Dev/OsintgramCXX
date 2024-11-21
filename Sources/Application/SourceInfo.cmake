CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_executable(OsintgramCXX ${ModSources})
target_link_libraries(OsintgramCXX PRIVATE ${OsintgramCXX_LINK_DEPS} OsintgramCXX-commons OsintgramCXX-networking)

set_target_properties(OsintgramCXX PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
set_target_properties(OsintgramCXX PROPERTIES INSTALL_RPATH '$ORIGIN')