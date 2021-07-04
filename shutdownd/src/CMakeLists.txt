cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")

    add_executable(Deadeye_shutdownd
        main.cpp
    )
    add_executable(Deadeye::shutdownd ALIAS Deadeye_shutdownd)
    set_target_properties(Deadeye_shutdownd PROPERTIES
    OUTPUT_NAME shutdownd
    EXPORT_NAME shutdownd
    )

    target_link_libraries(Deadeye_shutdownd
    PRIVATE
    libgpiodcxx.a
    libgpiod.a
    )

    include(GNUInstallDirs)
    install(TARGETS Deadeye_shutdownd DESTINATION ${CMAKE_INSTALL_BINDIR})

endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")