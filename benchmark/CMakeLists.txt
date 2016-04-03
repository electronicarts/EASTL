#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info 
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EASTLBenchmarks CXX)
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

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
endif()

# Work around a crash bug that only occurs with MingW when compiled with optimizations enabled and a dynamic runtime.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND MINGW)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -static")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -static")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_BUILD_TYPE MATCHES "MinSizeRel" AND MINGW)
	message(FATAL_ERROR "FIXME: MinSizeRel on MingW's Clang fails to link.")
endif()

#-------------------------------------------------------------------------------------------
# Source files 
#-------------------------------------------------------------------------------------------
file(GLOB EASTLBENCHMARK_SOURCES "source/*.cpp" "../test/source/EASTLTestAllocator.cpp" "../test/source/EASTLTest.cpp")
set(SOURCES ${EASTLBENCHMARK_SOURCES})

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DEASTL_THREAD_SUPPORT_AVAILABLE=0)
add_definitions(-DEASTL_OPENSOURCE=1)
add_definitions(-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)  # silence std::hash_map deprecation warnings

#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EASTLBenchmarks ${EASTLBENCHMARK_SOURCES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(EASTLBenchmark_Libraries
    EASTL
    EATest)
target_link_libraries(EASTLBenchmarks ${EASTLBenchmark_Libraries} Threads::Threads)

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results. 
#-------------------------------------------------------------------------------------------
add_test(EASTLBenchmarkRuns EASTLBenchmarks)
set_tests_properties (EASTLBenchmarkRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

