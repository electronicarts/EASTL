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
set(HEADER_DIR "include/Common/EABase")
set(EABASE_SOURCES
    ${HEADER_DIR}/eabase.h
    ${HEADER_DIR}/eahave.h
    ${HEADER_DIR}/earesult.h
    ${HEADER_DIR}/eastdarg.h
    ${HEADER_DIR}/nullptr.h
    ${HEADER_DIR}/version.h
    ${HEADER_DIR}/config/eacompiler.h
    ${HEADER_DIR}/config/eacompilertraits.h
    ${HEADER_DIR}/config/eaplatform.h
)

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
