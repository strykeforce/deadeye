add_executable(deadeye
    link/nt_link.cpp
    controller.cpp
    main.cpp
)

target_link_libraries(deadeye
    PRIVATE
    ntcore
    spdlog::spdlog
    tinyfsm
)
