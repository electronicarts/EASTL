#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EAThread CXX)

#-------------------------------------------------------------------------------------------
# Library definition
#-------------------------------------------------------------------------------------------
add_library(EAThread INTERFACE)

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EATHREAD_SOURCES "include/eathread/*.h")

#-------------------------------------------------------------------------------------------
# Include directories
#-------------------------------------------------------------------------------------------
target_include_directories(EAThread INTERFACE include)

# Workaround to have header files show up in IDEs
add_custom_target(EAThread_ide SOURCES ${EATHREAD_SOURCES})
