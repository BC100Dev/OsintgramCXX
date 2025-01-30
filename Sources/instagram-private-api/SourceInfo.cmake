CollectSources("${PROJECT_MODULE_ROOT}" ModuleSources)

add_library(instagram-private-api SHARED ${ModuleSources})
target_link_libraries(instagram-private-api PRIVATE OsintgramCXX-networking)