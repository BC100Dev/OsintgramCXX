CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(OsintgramCXX-commons SHARED ${ModSources})
target_link_libraries(OsintgramCXX-commons PRIVATE ${OsintgramCXX_LINK_DEPS})

if (APP_SYSTEM_TARGET STREQUAL "Windows")
    target_link_libraries(OsintgramCXX-commons PRIVATE ole32 shell32)
endif ()