#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EAStdC CXX)

#-------------------------------------------------------------------------------------------
# Library definition 
#-------------------------------------------------------------------------------------------
set(HEADER_DIR "include/EAStdC")
set(SOURCE_DIR "source")
set(EASTDC_SOURCES
    ${HEADER_DIR}/EAAlignment.h
    ${HEADER_DIR}/EADateTime.h
    ${HEADER_DIR}/EAMemory.h
    ${HEADER_DIR}/EAProcess.h
    ${HEADER_DIR}/EASprintf.h
    ${HEADER_DIR}/EAStopwatch.h
    ${HEADER_DIR}/EAString.h
    ${HEADER_DIR}/EATextUtil.h
    ${SOURCE_DIR}/EAMemory.cpp
    ${SOURCE_DIR}/EASprintf.cpp
)
add_library(EAStdC ${EASTDC_SOURCES})

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

#-------------------------------------------------------------------------------------------
# Include directories 
#-------------------------------------------------------------------------------------------
target_include_directories(EAStdC PUBLIC include)

#-------------------------------------------------------------------------------------------
# Dependencies 
#-------------------------------------------------------------------------------------------
target_link_libraries(EAStdC EABase)
target_link_libraries(EAStdC EAAssert)

