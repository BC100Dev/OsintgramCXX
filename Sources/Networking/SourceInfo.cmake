CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(OsintgramCXX-networking SHARED ${ModSources})
target_link_libraries(OsintgramCXX-networking PRIVATE ${OsintgramCXX_LINK_DEPS} OsintgramCXX-commons)