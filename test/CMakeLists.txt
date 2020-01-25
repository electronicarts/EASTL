#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EASTLTest CXX)
include(CheckCXXCompilerFlag)
include(CTest)

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(EASTL_BUILD_CHAR8T_TESTS "Enable building individual tests with char8_t support." OFF)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DEASTL_OPENSOURCE=1)
add_definitions(-D_CHAR16T)
add_definitions(-DEASTL_THREAD_SUPPORT_AVAILABLE=0)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../scripts/CMake")
include(CommonCppFlags)

if (MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-pointer-bool-conversion -Wno-unknown-warning-option")
endif()

# Temporary logic to build a subset of tests with char8_t support since the
# benchmark code and underlying submodules (particularly EAStdC) don't
# currently compile if the full project is built with these flags enabled.
if (EASTL_BUILD_CHAR8T_TESTS)
    check_cxx_compiler_flag(-fchar8_t COMPILER_SUPPORTS_GNU_CHAR8T_FLAG)
    check_cxx_compiler_flag(/Zc:char8_t COMPILER_SUPPORTS_MSVC_CHAR8T_FLAG)

    if (COMPILER_SUPPORTS_GNU_CHAR8T_FLAG)
        message(STATUS "Building with char8_t support in tests.")
        set_source_files_properties("source/TestString.cpp" "source/TestStringView.cpp" PROPERTIES COMPILE_FLAGS -fchar8_t)
    elseif (COMPILER_SUPPORTS_MSVC_CHAR8T_FLAG)
        message(STATUS "Building with char8_t support in tests.")
        set_source_files_properties("source/TestString.cpp" "source/TestStringView.cpp" PROPERTIES COMPILE_FLAGS /Zc:char8_t)
    else()
        message(NOTICE "Cannot build with char8_t support in tests: compiler does not support any known flags.")
    endif()
endif()

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EASTLTEST_SOURCES "source/*.cpp" "source/*.inl" "source/*.h")
set(SOURCES ${EASTLTEST_SOURCES})


#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EASTLTest ${SOURCES})

#-------------------------------------------------------------------------------------------
# Include directories
#-------------------------------------------------------------------------------------------
target_include_directories(EASTLTest PUBLIC include)

#-------------------------------------------------------------------------------------------
# Dependencies 
#-------------------------------------------------------------------------------------------
add_subdirectory(packages/EABase)
add_subdirectory(packages/EAAssert)
add_subdirectory(packages/EAStdC)
add_subdirectory(packages/EAMain)
add_subdirectory(packages/EATest)
add_subdirectory(packages/EAThread)

target_link_libraries(EASTLTest EABase)
target_link_libraries(EASTLTest EAAssert)
target_link_libraries(EASTLTest EAMain)
target_link_libraries(EASTLTest EASTL)
target_link_libraries(EASTLTest EAStdC)
target_link_libraries(EASTLTest EATest)
target_link_libraries(EASTLTest EAThread)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if((NOT APPLE) AND (NOT WIN32))
    target_link_libraries(EASTLTest ${EASTLTest_Libraries} Threads::Threads rt)
else()
    target_link_libraries(EASTLTest ${EASTLTest_Libraries} Threads::Threads)
endif()

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results.
#-------------------------------------------------------------------------------------------
add_test(EASTLTestRuns EASTLTest)
set_tests_properties (EASTLTestRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

