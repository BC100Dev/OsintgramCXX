CollectSources("${PROJECT_MODULE_ROOT}/cmd_core" CORE_SRC)
CollectSources("${PROJECT_MODULE_ROOT}/cmd_insta" INSTA_SRC)

add_library(OsintgramCXX-core SHARED "${PROJECT_MODULE_ROOT}/Commands.h" ${CORE_SRC})
add_library(OsintgramCXX-interactive SHARED "${PROJECT_MODULE_ROOT}/Commands.h" ${INSTA_SRC})

target_link_libraries(OsintgramCXX-core PRIVATE ${OsintgramCXX_LINK_DEPS})
target_link_libraries(OsintgramCXX-interactive PRIVATE ${OsintgramCXX_LINK_DEPS})