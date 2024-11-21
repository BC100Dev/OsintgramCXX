CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(OsintgramCXX-networking STATIC ${ModSources})
target_link_libraries(OsintgramCXX-networking PRIVATE ${OsintgramCXX_LINK_DEPS})