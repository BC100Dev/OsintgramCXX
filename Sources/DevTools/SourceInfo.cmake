CollectSources("${PROJECT_MODULE_ROOT}/Commons" CommonsSources)
CollectSources("${PROJECT_MODULE_ROOT}/net" NetSources)
CollectSources("${PROJECT_MODULE_ROOT}/executables" ExecutableSources)

add_library(devtools SHARED ${CommonsSources} ${NetSources})
target_link_libraries(devtools PRIVATE ${OsintgramCXX_LINK_DEPS})

if (APP_SYSTEM_TARGET STREQUAL "Windows")
    target_link_libraries(devtools PRIVATE ole32 shell32 uuid)
endif ()

if (DEFINED DEVTOOLS_ENABLE_EXECUTABLES AND DEVTOOLS_ENABLE_EXECUTABLES)
    foreach (ExecutableSource ${ExecutableSources})
        get_filename_component(EXEC_NAME "${ExecutableSource}" NAME_WE)
        add_executable(${EXEC_NAME} ${ExecutableSource})
        DisableBionicFortify(${EXEC_NAME})
        SetRpathValue(${EXEC_NAME})
        target_link_libraries(${EXEC_NAME} PRIVATE devtools)
        set_target_properties(${EXEC_NAME} PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_DEVTOOLS}"
                        LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_DEVTOOLS}"
                        ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_DEVTOOLS}")
    endforeach ()

    add_custom_command(TARGET devtools
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:devtools>"
            "${OUTPUT_DIRECTORY_DEVTOOLS}/$<TARGET_FILE_NAME:devtools>"
            COMMENT "Copying $<TARGET_FILE_NAME:devtools> -> ${OUTPUT_DIRECTORY_DEVTOOLS}/$<TARGET_FILE_NAME:devtools>")
endif ()