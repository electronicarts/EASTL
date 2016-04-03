#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EATest CXX)

#-------------------------------------------------------------------------------------------
# Library definition 
#-------------------------------------------------------------------------------------------
file(GLOB EATEST_SOURCES "source/*.cpp" "include/EATest/*.*" "include/EATest/internal/*.h")
add_library(EATest ${EATEST_SOURCES})

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CHAR16T)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
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

if(APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdeprecated")
endif()


#-------------------------------------------------------------------------------------------
# Include directories 
#-------------------------------------------------------------------------------------------
target_include_directories(EATest PUBLIC include)
target_link_libraries(EATest EABase)
target_link_libraries(EATest EAStdC)
target_link_libraries(EATest EASTL)
target_link_libraries(EATest EAMain)
target_link_libraries(EATest EAThread)
