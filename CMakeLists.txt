cmake_minimum_required(VERSION 3.16)
project(potto)

option(POTTO_BUILD_DEMO "Build the test project" ON)
option(POTTO_BUILD_STATIC "Build potto as static library" OFF)

# detect target platform
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(OS_WINDOWS TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(OS_LINUX TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(OS_ANDROID TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OS_APPLE TRUE)
    set(OS_MACOS TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(OS_APPLE TRUE)
    set(OS_IOS TRUE)
endif()

# detect compiler
# https://cmake.org/cmake/help/v3.0/variable/CMAKE_LANG_COMPILER_ID.html
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    set(COMPILER_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    set(COMPILER_GNU TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    # MSVC is also available, defined by CMake itself
    set(COMPILER_MSVC TRUE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# detect whether C++ 17 is supported or not
list(FIND CMAKE_CXX_COMPILE_FEATURES cxx_std_17 CXX_17_SUPPORTED)

if(CXX_17_SUPPORTED)
    # use c+++ 17 standard
    set(CMAKE_CXX_STANDARD 17)
    add_definitions(-DCXX_17_ENABLED)
else()
    # use c+++ 11 standard
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(OS_WINDOWS)
    if(COMPILER_MSVC)
        add_compile_options(
            /utf-8
        )
    elseif(COMPILER_CLANG)
        # configure for Clang
        add_compile_options(
        )
    elseif(COMPILER_GNU)
        # configure for Gcc
        add_compile_options(
        )
    endif()
else()
    add_compile_options(
        -g
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
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/potto
    PREFIX include
    FILES ${potto_INCLUDE_FILES}
)

# ######################################################
# create potto library
# Collect source files
file(GLOB_RECURSE potto_SRC_FILES
    src/*.h
    src/*.cpp
)
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${potto_SRC_FILES}
)

if(POTTO_BUILD_STATIC)
    add_library(${PROJECT_NAME} STATIC
        ${potto_INCLUDE_FILES}
        ${potto_SRC_FILES}
    )
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
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

if(NOT OS_WINDOWS)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        dl
    )
endif()

install(DIRECTORY include/
    CONFIGURATIONS Release
    DESTINATION potto/include
)
install(DIRECTORY include/
    CONFIGURATIONS Debug
    DESTINATION debug/potto/include
)
install(TARGETS ${PROJECT_NAME}
    CONFIGURATIONS Release
    ARCHIVE DESTINATION potto/lib
    LIBRARY DESTINATION potto/bin
    RUNTIME DESTINATION potto/bin
)
install(TARGETS ${PROJECT_NAME}
    CONFIGURATIONS Debug
    ARCHIVE DESTINATION debug/potto/lib
    LIBRARY DESTINATION debug/potto/bin
    RUNTIME DESTINATION debug/potto/bin
)

add_subdirectory(tools)

# add demo project
if(POTTO_BUILD_DEMO)
    add_subdirectory(example)
endif()
