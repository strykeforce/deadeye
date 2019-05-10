add_executable(deadeye
    camera.cpp
    lights.cpp
    controller.cpp
    main.cpp
)

target_link_libraries(deadeye
    PRIVATE
    ntcore
    spdlog::spdlog
    tinyfsm
    hedley
)
