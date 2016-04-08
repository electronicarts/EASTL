#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EASTL)

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(EASTL_BUILD_BENCHMARK "Enable generation of build files for benchmark" OFF)
option(EASTL_BUILD_TESTS "Enable generation of build files for tests" OFF)

#-------------------------------------------------------------------------------------------
# Sub-projects
#-------------------------------------------------------------------------------------------
add_subdirectory(test/packages/EABase)

if(EASTL_BUILD_TESTS OR EASTL_BUILD_BENCHMARK)
    add_subdirectory(test/packages/EAAssert)
    add_subdirectory(test/packages/EAStdC)
    add_subdirectory(test/packages/EAMain)
    add_subdirectory(test/packages/EATest)
    add_subdirectory(test/packages/EAThread)
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

#-------------------------------------------------------------------------------------------
# Library definition
#-------------------------------------------------------------------------------------------
file(GLOB EASTL_SOURCES "source/*.cpp" "include/EASTL/*.h")
add_library(EASTL ${EASTL_SOURCES})

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set_property(TARGET EASTL PROPERTY CXX_STANDARD 11)

if(EASTL_BUILD_TESTS OR EASTL_BUILD_BENCHMARK)
	set_property(TARGET EAStdC EATest PROPERTY CXX_STANDARD 11)
endif()

if( UNIX AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fasm-blocks" )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
endif()

#-------------------------------------------------------------------------------------------
# Include dirs
#-------------------------------------------------------------------------------------------
target_include_directories(EASTL PUBLIC include)

#-------------------------------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------------------------------
target_link_libraries(EASTL EABase)

#-------------------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------------------
install(TARGETS EASTL DESTINATION lib)
install(DIRECTORY include/EASTL DESTINATION include)
install(DIRECTORY test/packages/EABase/include/Common/EABase DESTINATION include)
