add_executable(deadeye
    main.cpp
    controller.cpp
    camera.cpp
    default_pipeline.cpp
    lights.cpp
)

target_link_libraries(deadeye
    PRIVATE
    ntcore
    cscore
    spdlog::spdlog
    tinyfsm
)
