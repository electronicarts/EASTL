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
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../scripts/CMake")
include(CommonCppFlags)

# Libstdc++ calls new internally, since DLLs have no weak symbols, runtime symbol resolution fails and EASTL's new is not called.
# Linking against static libstdc++ fixes this.
# See https://github.com/electronicarts/EASTL/issues/40 for more info.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND MINGW)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} -static-libstdc++")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_BUILD_TYPE MATCHES "MinSizeRel" AND MINGW)
    message(FATAL_ERROR "FIXME: MinSizeRel on MingW-w64's Clang fails to link.")
endif()

# The benchmark suite fails to compile if char8_t is enabled, so disable it.
if (EASTL_NO_CHAR8T_FLAG)
    add_compile_options(${EASTL_NO_CHAR8T_FLAG})
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

if(NOT EASTL_BUILD_TESTS)
    add_subdirectory(../test/packages/EAStdC ../test/EAStdC)
    add_subdirectory(../test/packages/EAAssert ../test/EAAssert)
    add_subdirectory(../test/packages/EAThread ../test/EAThread)
    add_subdirectory(../test/packages/EATest ../test/EATest)
    add_subdirectory(../test/packages/EAMain ../test/EAMain)
endif()

#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EASTLBenchmarks ${EASTLBENCHMARK_SOURCES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(EASTLBenchmark_Libraries
    EABase
    EAAssert
    EAMain
    EAThread
    EAStdC
    EASTL
    EATest)
target_link_libraries(EASTLBenchmarks ${EASTLBenchmark_Libraries} Threads::Threads)

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results.
#-------------------------------------------------------------------------------------------
add_test(EASTLBenchmarkRuns EASTLBenchmarks)
set_tests_properties (EASTLBenchmarkRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

