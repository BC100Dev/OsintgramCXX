cmake_minimum_required(VERSION 3.23)
project(OsintgramCXX)

set(CMAKE_CXX_STANDARD 23)

file(GLOB CM_CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CM_Config/*.cmake")
foreach (CM_CONFIG_FILE ${CM_CONFIG_FILES})
    include("${CM_CONFIG_FILE}")
endforeach ()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")

file(GLOB Private_CM_Modules "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*")
foreach (Private_CM_Module ${Private_CM_Modules})
    if (EXISTS "${Private_CM_Module}/SourceInfo.cmake")
        set(PROJECT_MODULE_ROOT "${Private_CM_Module}")
        include("${PROJECT_MODULE_ROOT}/SourceInfo.cmake")

        message(STATUS "Project Index Successful: ${PROJECT_MODULE_ROOT}")
    else ()
        message(STATUS "Project Index Ignore: ${Private_CM_Module}")
    endif ()
endforeach ()
