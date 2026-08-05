CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(app-shell SHARED ${ModSources})
target_link_libraries(app-shell PRIVATE ${OsintgramCXX_LINK_DEPS} devtools)