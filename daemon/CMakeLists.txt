# Preamble
cmake_minimum_required(VERSION 3.10)
include(version.cmake)
project(Deadeye LANGUAGES CXX VERSION ${Deadeye_VER})

find_package(Git REQUIRED)

execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list -1 HEAD -- ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
    RESULT_VARIABLE result
    OUTPUT_VARIABLE lastChangeHash
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(result)
    message(FATAL_ERROR "Failed to get hash of last change: ${result}")
endif()

execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list ${lastChangeHash}..HEAD
    RESULT_VARIABLE result
    OUTPUT_VARIABLE hashList
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(result)
    message(FATAL_ERROR "Failed to get list of git hashes: ${result}")
endif()

string(REGEX REPLACE "[\n\r]+" ";" hashList "${hashList}")
list(LENGTH hashList Deadeye_COMMITS_SINCE_VERSION_CHANGE)

if(NOT CMAKE_CROSSCOMPILING)
    enable_testing()
endif(NOT CMAKE_CROSSCOMPILING)

# Project-wide setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS NO)

# Build options
set(DEADEYE_UNIT_ID "A" CACHE STRING "Deadeye unique unit ID")
set(DEADEYE_NT_SERVER "10.27.67.2" CACHE STRING "NetworkTables server address")
set(DEADEYE_CLIENT_ADDRESS "10.27.67.2" CACHE STRING "Deadeye client address")
set(DEADEYE_CLIENT_PORT "5800" CACHE STRING "Deadeye client port")

# Camera Pipelines
set(DEADEYE_CAMERA0_PIPELINE "deadeye::DefaultPipeline" CACHE STRING "Camera 0 Pipeline class")
set(DEADEYE_CAMERA1_PIPELINE "" CACHE STRING "Camera 1 Pipeline class")
set(DEADEYE_CAMERA2_PIPELINE "" CACHE STRING "Camera 2 Pipeline class")
set(DEADEYE_CAMERA3_PIPELINE "" CACHE STRING "Camera 3 Pipeline class")
set(DEADEYE_CAMERA4_PIPELINE "" CACHE STRING "Camera 4 Pipeline class")

# Dependencies
find_package(spdlog REQUIRED)
find_package(ntcore REQUIRED)
find_package(cscore REQUIRED)
find_package(Threads REQUIRED) # required for ntcore
find_package(nlohmann_json REQUIRED)

if(CMAKE_CROSSCOMPILING)
    set(ENV{PKG_CONFIG_DIR} "")
    set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
    set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/local/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig")
endif(CMAKE_CROSSCOMPILING)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SYSTEMD REQUIRED IMPORTED_TARGET libsystemd)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiodcxx)
endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")

add_subdirectory(dependencies)

get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
    set(enableBuildMain YES)
    if(NOT CMAKE_CROSSCOMPILING)
        find_package(Catch2 REQUIRED)
        add_subdirectory(tests)
    endif(NOT CMAKE_CROSSCOMPILING)
endif(NOT hasParent)


# Main target
add_subdirectory(src)

# Only if top-level project
