if (NOT DEFINED OsintgramCXX_CompileTestCode)
    return()
endif ()

if (OsintgramCXX_CompileTestCode EQUAL ON)
    message(STATUS "Ignore Testing Source Code")
    return()
endif ()

CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_executable(OsintgramCXX-testing ${ModSources})

target_link_libraries(OsintgramCXX-testing PRIVATE ${OsintgramCXX_LINK_DEPS} OsintgramCXX-networking OsintgramCXX-security OsintgramCXX-commons)