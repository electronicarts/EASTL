#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Library definition
#-------------------------------------------------------------------------------------------
file(GLOB EASTL_SOURCES "source/*.cpp" "include/EASTL/*.h")
add_library(EASTL ${EASTL_SOURCES})

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set_property(TARGET EASTL PROPERTY CXX_STANDARD 11)
if( UNIX AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fasm-blocks" )
endif()

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

#-------------------------------------------------------------------------------------------
# Include dirs
#-------------------------------------------------------------------------------------------
target_include_directories(EASTL PUBLIC include ${PROJECT_SOURCE_DIR}/test/packages/EABase/include/Common)
target_link_libraries(EASTL EABase)

#-------------------------------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------------------------------
add_subdirectory(test/packages/EAAssert)
add_subdirectory(test/packages/EABase)
add_subdirectory(test/packages/EAStdC)
add_subdirectory(test/packages/EAMain)
add_subdirectory(test/packages/EAThread)

set(EASTL_Libraries
    EAAssert
    EABase
    EAMain
    EAStdC
    EAThread)
target_link_libraries(EASTL ${EASTL_Libraries})

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DEASTL_OPENSOURCE)

#-------------------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------------------
install (TARGETS EASTL DESTINATION lib)
install (DIRECTORY include/EASTL DESTINATION include)
install (DIRECTORY test/packages/EABase/include/Common/EABase DESTINATION include)

