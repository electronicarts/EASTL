#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.11)
project(EASTLTest CXX)
include(CTest)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DEASTL_OPENSOURCE=1)
add_definitions(-D_CHAR16T)
add_definitions(-DEASTL_THREAD_SUPPORT_AVAILABLE=0)
if (EASTL_STD_ITERATOR_CATEGORY_ENABLED)
  add_definitions(-DEASTL_STD_ITERATOR_CATEGORY_ENABLED=1)
endif()

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

# Parts of the test suite fail to compile if char8_t is enabled, so we
# disable it and only enable for specific source files later on.
if (EASTL_NO_CHAR8T_FLAG)
    add_compile_options(${EASTL_NO_CHAR8T_FLAG})
endif()

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EASTLTEST_SOURCES "source/*.cpp")
file(GLOB EASTLTEST_HEADERS "source/*.inl" "source/*.h")
set(SOURCES ${EASTLTEST_SOURCES} ${EASTLTEST_HEADERS})

# Compile a subset of tests with explicit char8_t support if available.
if (EASTL_CHAR8T_FLAG)
    message(STATUS "Building with char8_t support in tests.")
    set(EASTLTEST_CHAR8T_SOURCES "source/TestString.cpp" "source/TestStringView.cpp")

    set_source_files_properties(${EASTLTEST_CHAR8T_SOURCES} PROPERTIES
        COMPILE_FLAGS ${EASTL_CHAR8T_FLAG}
        COMPILE_DEFINITIONS "EASTL_EXPECT_CHAR8T_SUPPORT")
endif()

# include both source and headers in the files view in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${EASTLTEST_HEADERS})

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
FetchContent_Declare(
  EABase
  GIT_REPOSITORY https://github.com/electronicarts/EABase.git
  GIT_TAG        123363eb82e132c0181ac53e43226d8ee76dea12
  GIT_SUBMODULES "" # This should be temporary until we update the cyclic submodule dependencies in EABase.
)
FetchContent_MakeAvailable(EABase)
target_link_libraries(EASTLTest EABase)

FetchContent_Declare(
  EAAssert
  GIT_REPOSITORY https://github.com/electronicarts/EAAssert.git
  GIT_TAG        e5e181255de2e883dd1f987c78ccc42ac81d3bca
  GIT_SUBMODULES "" # This should be temporary until we update the cyclic submodule dependencies in EAAssert.
)
FetchContent_MakeAvailable(EAAssert)
target_link_libraries(EASTLTest EAAssert)

FetchContent_Declare(
  EAStdC
  GIT_REPOSITORY https://github.com/electronicarts/EAStdC.git
  GIT_TAG        fbcc34e89c63636054334888f3a5bd7ac2fd4b76
  GIT_SUBMODULES "" # This should be temporary until we update the cyclic submodule dependencies in EAStdC.
)
FetchContent_MakeAvailable(EAStdC)
target_link_libraries(EASTLTest EAStdC)

FetchContent_Declare(
  EAMain
  GIT_REPOSITORY https://github.com/electronicarts/EAMain.git
  GIT_TAG        24ca8bf09e6b47b860286fc2f4c832f4009273d1
  GIT_SUBMODULES "" # This should be temporary until we update the cyclic submodule dependencies in EAMain.
)
FetchContent_MakeAvailable(EAMain)
target_link_libraries(EASTLTest EAMain)

FetchContent_Declare(
  EATest
  GIT_REPOSITORY https://github.com/electronicarts/EATest.git
  GIT_TAG        a59b372fc9cba517283ad6d060d2ab96e0ba34ac
  GIT_SUBMODULES "" # This should be temporary until we update the cyclic submodule dependencies in EATest.
)
FetchContent_MakeAvailable(EATest)
target_link_libraries(EASTLTest EATest)

FetchContent_Declare(
  EAThread
  GIT_REPOSITORY https://github.com/electronicarts/EAThread.git
  GIT_TAG        f3c6c54d19699639a5edcf5237ea8b71aca6842c
  GIT_SUBMODULES "" # This should be temporary until we update the cyclic submodule dependencies in EAThread.
)
FetchContent_MakeAvailable(EAThread)
target_link_libraries(EASTLTest EAThread)

target_link_libraries(EASTLTest EASTL)

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

