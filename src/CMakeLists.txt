add_executable(deadeye
    camera.cpp
    pipeline.cpp
    lights.cpp
    controller.cpp
    main.cpp
)

target_link_libraries(deadeye
    PRIVATE
    ntcore
    cscore
    spdlog::spdlog
    tinyfsm
    hedley
)
