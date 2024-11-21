CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_executable(OsintgramCXX-testing ${ModSources})
target_link_libraries(OsintgramCXX-testing PRIVATE OsintgramCXX-networking OsintgramCXX-commons)