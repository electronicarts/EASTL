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
add_definitions(-DEASTL_OPENSOURCE=1)
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Include directories
#-------------------------------------------------------------------------------------------
include_directories(source "../include/Common")

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EABASETEST_SOURCES "source/*.cpp" "source/*.inl" "source/*.h")
set(SOURCES ${EABASETEST_SOURCES})

#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EABaseTest ${SOURCES})

#-------------------------------------------------------------------------------------------
# Dependency include paths
# NOTE(rparolin): Why can't this be in the package build script?
#-------------------------------------------------------------------------------------------
target_include_directories(EABaseTest PRIVATE packages/EAAssert/include)
target_include_directories(EABaseTest PRIVATE packages/EAMain/include)
target_include_directories(EABaseTest PRIVATE packages/EASTL/include)
target_include_directories(EABaseTest PRIVATE packages/EAStdC/include)
target_include_directories(EABaseTest PRIVATE packages/EATest/include)

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results.
#-------------------------------------------------------------------------------------------
add_test(EABaseTestRuns EABaseTest)
set_tests_properties (EABaseTestRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

