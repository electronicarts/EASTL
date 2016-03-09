#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EABase CXX)

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EABASE_SOURCES "include/Common/EABase/*.h" "include/Common/EABase/config/*.h")

#-------------------------------------------------------------------------------------------
# Header only library
#-------------------------------------------------------------------------------------------
add_library(EABase INTERFACE)

#-------------------------------------------------------------------------------------------
# exported include directories 
#-------------------------------------------------------------------------------------------
target_include_directories(EABase INTERFACE include/Common)

# HACK: Workaround CMake limitations to have files of interface / header-only
#       libaries show up in IDEs
# http://stackoverflow.com/questions/27039019/how-to-have-cmake-show-headers-that-are-not-part-of-any-binary-target-in-the-ide/29218394#29218394
add_custom_target(EABase_ide SOURCES ${EABASE_SOURCES})
