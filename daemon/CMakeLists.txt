# Preamble
cmake_minimum_required(VERSION 3.10)
project(Deadeye VERSION 0.1 LANGUAGES CXX)
enable_testing()

# Project-wide setup
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS NO)

# Build options
set(DEADEYE_UNIT_ID "A" CACHE STRING "Deadeye unique unit ID")
set(DEADEYE_CAMERA0_PIPELINE "TestPatternPipeline" CACHE STRING "Camera 0 Pipeline class")
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

add_subdirectory(dependencies)

# Main target
add_subdirectory(src)

# Only if top-level project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
find_package(Catch2 REQUIRED)
add_subdirectory(tests)
endif()