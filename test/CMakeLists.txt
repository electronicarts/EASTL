#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info 
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EASTLTest)
include(CTest)

#-------------------------------------------------------------------------------------------
# Include directories 
#-------------------------------------------------------------------------------------------
include_directories(source)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -D_DEBUG")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -g -D_DEBUG")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Disable some (maybe useless) warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shift-negative-value -Wno-pointer-bool-conversion")
endif()

#-------------------------------------------------------------------------------------------
# Libraries 
#-------------------------------------------------------------------------------------------
add_subdirectory(packages/EATest)
add_subdirectory(../ "${CMAKE_CURRENT_BINARY_DIR}/eastl")

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EASTLTEST_SOURCES "source/*.cpp")
set(SOURCES ${EASTLTEST_SOURCES})

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DEASTL_OPENSOURCE)
add_definitions(-D_CHAR16T)
add_definitions(-DEASTL_THREAD_SUPPORT_AVAILABLE=0)
target_compile_definitions(EASTL PUBLIC -DEASTL_THREAD_SUPPORT_AVAILABLE=0)

#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EASTLTest ${SOURCES})

set(EASTLTest_Libraries
    EASTL
    EATest)
target_link_libraries(EASTLTest ${EASTLTest_Libraries})

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results. 
#-------------------------------------------------------------------------------------------
add_test(EASTLTestRuns EASTLTest)
set_tests_properties (EASTLTestRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

