#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EASTL CXX)

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(EASTL_BUILD_BENCHMARK "Enable generation of build files for benchmark" OFF)
option(EASTL_BUILD_TESTS "Enable generation of build files for tests" OFF)
option(EASTL_STD_ITERATOR_CATEGORY_ENABLED "Enable compatibility with std:: iterator categories" OFF)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts/CMake")
include(CommonCppFlags)

#-------------------------------------------------------------------------------------------
# Library definition
#-------------------------------------------------------------------------------------------
file(GLOB EASTL_SOURCES "source/*.cpp")
add_library(EASTL ${EASTL_SOURCES})

if (MSVC)
    set(EASTL_NATVIS_DIR "doc")
    set(EASTL_NATVIS_FILE "${EASTL_NATVIS_DIR}/EASTL.natvis")
    target_sources(EASTL INTERFACE
        $<INSTALL_INTERFACE:${EASTL_NATVIS_FILE}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${EASTL_NATVIS_FILE}>
    )
endif()

if(EASTL_BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()

if(EASTL_BUILD_TESTS)
    add_subdirectory(test)
endif()

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CHAR16T)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DEASTL_OPENSOURCE=1)
if (EASTL_STD_ITERATOR_CATEGORY_ENABLED)
  add_definitions(-DEASTL_STD_ITERATOR_CATEGORY_ENABLED=1)
endif()

#-------------------------------------------------------------------------------------------
# Include dirs
#-------------------------------------------------------------------------------------------
target_include_directories(EASTL PUBLIC include)

#-------------------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------------------
if (NOT TARGET EABase)
    add_subdirectory(test/packages/EABase)
endif()

target_link_libraries(EASTL EABase)

#-------------------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------------------
install(TARGETS EASTL DESTINATION lib)
install(DIRECTORY include/EASTL DESTINATION include)

if (MSVC)
    install(FILES ${EASTL_NATVIS_FILE} DESTINATION ${EASTL_NATVIS_DIR})
endif()
