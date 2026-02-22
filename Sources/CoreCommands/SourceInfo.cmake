CollectSources("${PROJECT_MODULE_ROOT}/cmd_core" CORE_SRC)
CollectSources("${PROJECT_MODULE_ROOT}/cmd_insta" INSTA_SRC)
CollectSources("${PROJECT_MODULE_ROOT}/shared" SHARED_SRC)

add_library(OsintgramCXX-core SHARED ${CORE_SRC} ${SHARED_SRC})
add_library(OsintgramCXX-interactive SHARED ${INSTA_SRC} ${SHARED_SRC})

target_link_libraries(OsintgramCXX-core PRIVATE ${OsintgramCXX_LINK_DEPS} devtools instagram-private-api)
target_include_directories(OsintgramCXX-core PRIVATE "${PROJECT_MODULE_ROOT}/shared")
DisableBionicFortify(OsintgramCXX-core)

target_link_libraries(OsintgramCXX-interactive PRIVATE ${OsintgramCXX_LINK_DEPS} devtools instagram-private-api)
target_include_directories(OsintgramCXX-interactive PRIVATE "${PROJECT_MODULE_ROOT}/shared")
DisableBionicFortify(OsintgramCXX-interactive)