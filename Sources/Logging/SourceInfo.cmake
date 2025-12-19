CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(app-logger SHARED ${ModSources})
target_link_libraries(app-logger PRIVATE OsintgramCXX-commons)