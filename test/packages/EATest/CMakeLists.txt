#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Library definition 
#-------------------------------------------------------------------------------------------
file(GLOB EATEST_SOURCES "source/*.cpp")
add_library(EATest ${EATEST_SOURCES})

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
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

#-------------------------------------------------------------------------------------------
# Defines 
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

