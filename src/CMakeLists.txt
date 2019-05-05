add_executable(deadeye
    controller.cpp
    main.cpp
)

target_link_libraries(deadeye
    PRIVATE
    ntcore
    spdlog::spdlog
    tinyfsm
)
