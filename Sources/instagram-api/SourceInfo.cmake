CollectSources("${PROJECT_MODULE_ROOT}" ModuleSources)

add_library(instagram-api SHARED ${ModuleSources})
target_link_libraries(instagram-api PRIVATE ${OsintgramCXX_LINK_DEPS} ${OsintgramCXX_LINK_OpenSSL} devtools)
SetRpathValue(instagram-api)