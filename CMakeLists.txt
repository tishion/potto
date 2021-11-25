cmake_minimum_required(VERSION 3.16)
project(potto)

option(POTTO_BUILD_DEMO "Build the test project" OFF)
option(POTTO_STATIC_LIB "Build potto as static library" OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(ANDROID TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(IOS TRUE)
    if(${CMAKE_VERSION} VERSION_LESS "3.14.0") 
        message(FATAL_ERROR "CMake Version is too low to build for iOS platform, 3.14+ is required.")
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# use c+++ 11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WINDOWS)
    add_definitions(
    )
    add_compile_options(
        /utf-8
    )
endif()

# add third party dependencies
add_subdirectory(thirdparty)
include_directories(
    include
    thirdparty
)

# Collect include files
file(GLOB_RECURSE potto_INCLUDE_FILES
    include/*.h
    include/*.hpp
)
source_group(
    TREE ${CMAKE_SOURCE_DIR}/include/potto
    PREFIX include
    FILES ${potto_INCLUDE_FILES}
)

#######################################################
# create potto library
# Collect source files
file(GLOB_RECURSE potto_SRC_FILES
    src/*.h
    src/*.cpp
)
source_group(
    TREE ${CMAKE_SOURCE_DIR}
    FILES ${potto_SRC_FILES}
)

if (POTTO_STATIC_LIB)
    add_library(${PROJECT_NAME} STATIC
        ${potto_INCLUDE_FILES}
        ${potto_SRC_FILES}
    )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE 
            -DPOTTO_STATIC_LIB
    )
else()
    add_library(${PROJECT_NAME} SHARED
        ${potto_INCLUDE_FILES}
        ${potto_SRC_FILES}
    )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE 
            -DPOTTO_EXPORTS
    )    
endif()

add_subdirectory(tools)

# add demo project
if (POTTO_BUILD_DEMO)
    add_subdirectory(example)
endif()