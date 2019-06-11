#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EABaseTest CXX)
include(CTest)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/packages/EASTL/scripts/CMake")
include(CommonCppFlags)

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EABASETEST_SOURCES "source/TestEABase.cpp" "source/TestEABase.h")
set(SOURCES ${EABASETEST_SOURCES})

#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EABaseTest ${SOURCES})

#-------------------------------------------------------------------------------------------
# Dependencies 
#-------------------------------------------------------------------------------------------
add_subdirectory(packages/EAAssert)
add_subdirectory(packages/EAMain)
add_subdirectory(packages/EASTL)
add_subdirectory(packages/EAStdC)
add_subdirectory(packages/EATest)
add_subdirectory(packages/EAThread)

target_link_libraries(EABaseTest EAAssert)
target_link_libraries(EABaseTest EAMain)
target_link_libraries(EABaseTest EASTL)
target_link_libraries(EABaseTest EAStdC)
target_link_libraries(EABaseTest EATest)
target_link_libraries(EABaseTest EAThread)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if((NOT APPLE) AND (NOT WIN32))
    target_link_libraries(EABaseTest ${EASTLTest_Libraries} Threads::Threads rt)
else()
    target_link_libraries(EABaseTest ${EASTLTest_Libraries} Threads::Threads)
endif()

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results.
#-------------------------------------------------------------------------------------------
add_test(EABaseTestRuns EABaseTest)
set_tests_properties (EABaseTestRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

