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
