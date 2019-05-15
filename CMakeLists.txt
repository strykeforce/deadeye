cmake_minimum_required(VERSION 3.10)

project(deadeye VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(spdlog REQUIRED)
find_package(ntcore REQUIRED)
find_package(cscore REQUIRED)
find_package(Threads REQUIRED) # required for ntcore

add_subdirectory(vendor)
add_subdirectory(src)
