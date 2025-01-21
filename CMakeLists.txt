cmake_minimum_required(VERSION 3.23...3.28)

project(project_kwargs LANGUAGES C)

set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_BINARY_DIR}/dist")
endif()

add_subdirectory(${PROJECT_ROOT}/man/kwargs)
add_subdirectory(${PROJECT_ROOT}/src/kwargs)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # I'm the top-most project
    option(KWARGS_BUILD_EXAMPLES "Whether to build the libkwargs examples" ON)
else()
    # I'm not the top-most project, use KWARGS_BUILD_EXAMPLES as-is
    option(KWARGS_BUILD_EXAMPLES "Whether to build the libkwargs examples" OFF)
endif()
if (KWARGS_BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_ROOT}/src/example)
endif()
