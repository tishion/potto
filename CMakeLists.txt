cmake_minimum_required(VERSION 3.16)
project(potto)

option(POTTO_BUILD_DEMO "Build the test project" ON)
option(POTTO_BUILD_STATIC "Build potto as static library" OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(ANDROID TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(APPLE TRUE)
    set(MACOSX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(APPLE TRUE)
    set(IOS TRUE)
    if(${CMAKE_VERSION} VERSION_LESS "3.14.0") 
        message(FATAL_ERROR "CMake Version is too low to build for iOS platform, 3.14+ is required.")
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# use c+++ 11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_definitions(
    )
    add_compile_options(
        /utf-8
    )
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

if (POTTO_BUILD_STATIC)
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

if(NOT WINDOWS)
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
if (POTTO_BUILD_DEMO)
    add_subdirectory(example)
endif()
