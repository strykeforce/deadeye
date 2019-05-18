# Build as static library to facilitate testing
set(BUILD_SHARED_LIBS NO)

add_library(deadeye
    controller.cpp
    camera.cpp
    default_pipeline.cpp
    pipeline_config.cpp
    lights.cpp
)

target_include_directories(deadeye PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(deadeye
    PUBLIC
    ntcore
    cscore
    spdlog::spdlog
    tinyfsm
    nlohmann_json::nlohmann_json
)

# Build main target
add_executable(deadeyed
    main.cpp
)

target_link_libraries(deadeyed
    PRIVATE
    deadeye
)
