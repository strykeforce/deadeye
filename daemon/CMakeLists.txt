# Preamble
cmake_minimum_required(VERSION 3.10)
include(version.cmake)
project(Deadeye LANGUAGES CXX VERSION ${Deadeye_VER})

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
option(DEADEYE_BUILD_TESTS "Build tests" ON)
option(DEADEYE_BUILD_BENCHMARKS "Build benchmark tests" ON)

set(DEADEYE_UNIT_ID "A" CACHE STRING "Deadeye unique unit ID")
set(DEADEYE_NT_SERVER "10.27.67.2" CACHE STRING "NetworkTables server address")
set(DEADEYE_CLIENT_ADDRESS "10.27.67.2" CACHE STRING "Deadeye client address")
set(DEADEYE_CLIENT_PORT "5800" CACHE STRING "Deadeye client port")
set(DEADEYE_STREAM_ADDRESS "10.27.67.10" CACHE STRING "Deadeye stream server address")

# Camera Pipelines
set(DEADEYE_CAMERA0_PIPELINE "deadeye::UprightRectPipeline" CACHE STRING "Camera 0 Pipeline class")
set(DEADEYE_CAMERA1_PIPELINE "" CACHE STRING "Camera 1 Pipeline class")
set(DEADEYE_CAMERA2_PIPELINE "" CACHE STRING "Camera 2 Pipeline class")
set(DEADEYE_CAMERA3_PIPELINE "" CACHE STRING "Camera 3 Pipeline class")
set(DEADEYE_CAMERA4_PIPELINE "" CACHE STRING "Camera 4 Pipeline class")

# Create config.h and put in header search path
configure_file(
        "${PROJECT_SOURCE_DIR}/src/config.h.in"
        "${PROJECT_BINARY_DIR}/config.h"
)

# Dependencies
find_package(spdlog REQUIRED)
find_package(wpilib REQUIRED)
find_package(Threads REQUIRED) # required for ntcore
find_package(nlohmann_json REQUIRED)
find_package(safe REQUIRED)
find_package(tinyfsm REQUIRED)

if(CMAKE_CROSSCOMPILING)
    set(ENV{PKG_CONFIG_DIR} "")
    set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
    set(pkgconfigPath
        "${CMAKE_SYSROOT}/usr/lib/pkgconfig"
        "${CMAKE_SYSROOT}/usr/local/lib/pkgconfig"
        "${CMAKE_SYSROOT}/usr/share/pkgconfig"
        "${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu/pkgconfig"
    )
    string(REPLACE ";" ":" ENV{PKG_CONFIG_LIBDIR} "${pkgconfigPath}")
endif(CMAKE_CROSSCOMPILING)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SYSTEMD REQUIRED IMPORTED_TARGET libsystemd)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiodcxx)
endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")

get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
    set(enableBuildMain YES)
    if(DEADEYE_BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
        find_package(Catch2 REQUIRED)
        add_subdirectory(tests)
    endif(DEADEYE_BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
endif(NOT hasParent)


# Main target
add_subdirectory(src)

# Only if top-level project
