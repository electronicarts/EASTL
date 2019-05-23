#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EABase CXX)

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(EABASE_BUILD_TESTS "Enable generation of build files for tests" OFF)

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EABASE_SOURCES "include/Common/EABase/*.h" "include/Common/EABase/config/*.h")

if(EABASE_BUILD_TESTS)
    add_subdirectory(test/packages/EAAssert)
    add_subdirectory(test/packages/EAStdC)
    add_subdirectory(test/packages/EAMain)
    add_subdirectory(test/packages/EATest)
    add_subdirectory(test/packages/EAThread)
    add_subdirectory(test)
endif()

#-------------------------------------------------------------------------------------------
# Header only library
#-------------------------------------------------------------------------------------------
add_library(EABase INTERFACE)

#-------------------------------------------------------------------------------------------
# exported include directories 
#-------------------------------------------------------------------------------------------
target_include_directories(EABase INTERFACE include/Common)

