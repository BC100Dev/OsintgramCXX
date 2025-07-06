CollectSources("${PROJECT_MODULE_ROOT}/cmd_core" CORE_SRC)
CollectSources("${PROJECT_MODULE_ROOT}/cmd_insta" INSTA_SRC)

add_library(OsintgramCXX-core SHARED ${CORE_SRC})
add_library(OsintgramCXX-interactive SHARED ${INSTA_SRC})