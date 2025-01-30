CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(OsintgramCXX-commons SHARED ${ModSources})
target_link_libraries(OsintgramCXX-commons PRIVATE ${OsintgramCXX_LINK_DEPS})