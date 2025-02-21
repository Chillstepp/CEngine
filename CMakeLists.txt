cmake_minimum_required(VERSION 3.23)
project(CEngine)

set(CMAKE_CXX_STANDARD 20) #Some library required
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -Wall")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#define a macro "CE_DEFINE_RES_ROOT_DIR" which equals to "{CMAKE_SOURCE_DIR}/Resource/" dict.
add_definitions(-DCE_DEFINE_RES_ROOT_DIR=\"${CMAKE_SOURCE_DIR}/Resource/\")

if (WIN32)
    message("Platform: Windows")
    add_definitions(-DCE_ENGINE_PLATFORM_WIN32)
elseif(UNIX)
    if (APPLE)
        message("Platform: Macos")
        add_definitions(-DCE_ENGINE_PLATFORM_MACOS)
    else()
        message("Platform: Macos")
        add_definitions(-DCE_ENGINE_PLATFORM_LINUX)
    endif ()
else()
    message("Platform: Unsupported")
endif ()

#resource dir configuration
add_definitions(-DCE_DEFINE_RES_ROOT_DIR=\"${CMAKE_SOURCE_DIR}/Resource/\")
set(CE_DEFINE_RES_ROOT_DIR "${CMAKE_SOURCE_DIR}/Resource/")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SPIR-V)

include_directories(Platform/Public)
include_directories(Core/Public)

ADD_SUBDIRECTORY(Platform)
ADD_SUBDIRECTORY(Core)
ADD_SUBDIRECTORY(Editor)
ADD_SUBDIRECTORY(Sample)

#add_executable(CEngine main.cpp)
#重定义运行exe位置
#set_target_properties(CEngine PROPERTIES
#        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
#        )
