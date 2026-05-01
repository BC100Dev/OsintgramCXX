if (NOT DEFINED OsintgramCXX_CompileTestCode)
    return()
endif ()

if (NOT OsintgramCXX_CompileTestCode)
    message(STATUS "Ignore Testing Source Code")
    return()
endif ()

CollectSources(${PROJECT_MODULE_ROOT} ModSources)

add_executable(OsintgramCXX-testing ${ModSources})
SetRpathValue(OsintgramCXX-testing)
target_link_libraries(OsintgramCXX-testing PRIVATE ${OsintgramCXX_LINK_DEPS} OsintgramCXX-security devtools)
DisableBionicFortify(OsintgramCXX-testing)