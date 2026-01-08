CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_library(dev-utils SHARED ${ModSources})
target_link_libraries(dev-utils PRIVATE ${OsintgramCXX_LINK_DEPS})

if (APP_SYSTEM_TARGET STREQUAL "Windows")
    target_link_libraries(dev-utils PRIVATE ole32 shell32 uuid)
endif ()