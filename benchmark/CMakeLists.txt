#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info 
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EASTLBenchmarks)
include(CTest)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Include directories 
#-------------------------------------------------------------------------------------------
include_directories(source)
include_directories(../test/source)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
endif()

# Work around a crash bug that only occurs with MingW when compiled with optimizations enabled and a dynamic runtime.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static")
endif()

#-------------------------------------------------------------------------------------------
# Libraries 
#-------------------------------------------------------------------------------------------
add_subdirectory(../ EASTL)
add_subdirectory(../test/packages/EATest EATest)

#-------------------------------------------------------------------------------------------
# Source files 
#-------------------------------------------------------------------------------------------
file(GLOB EASTLBENCHMARK_SOURCES "source/*.cpp" "../test/source/EASTLTestAllocator.cpp" "../test/source/EASTLTest.cpp")
set(SOURCES ${EASTLBENCHMARK_SOURCES})

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DEASTL_THREAD_SUPPORT_AVAILABLE=0) 
add_definitions(-DEASTL_OPENSOURCE=1)
add_definitions(-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)  # silence std::hash_map deprecation warnings
target_compile_definitions(EASTL PUBLIC -DEASTL_THREAD_SUPPORT_AVAILABLE=0)


#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EASTLBenchmarks ${EASTLBENCHMARK_SOURCES})

set(EASTLBenchmark_Libraries
    EASTL
    EATest)
target_link_libraries(EASTLBenchmarks ${EASTLBenchmark_Libraries})

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results. 
#-------------------------------------------------------------------------------------------
add_test(EASTLBenchmarkRuns EASTLBenchmarks)
set_tests_properties (EASTLBenchmarkRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

