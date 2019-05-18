# Build library to facilitate testing
add_library(Deadeye_library
    controller.cpp
    camera.cpp
    default_pipeline.cpp
    pipeline_config.cpp
    lights.cpp
)
add_library(Deadeye::library ALIAS Deadeye_library)
set_target_properties(Deadeye_library PROPERTIES
  OUTPUT_NAME deadeye
  EXPORT_NAME deadeye
)

target_include_directories(Deadeye_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Deadeye_library
    PUBLIC
    ntcore
    cscore
    spdlog::spdlog
    tinyfsm
    nlohmann_json::nlohmann_json
)

# Build main target
add_executable(Deadeye_deadeyed
    main.cpp
)
add_executable(Deadeye::deadeyed ALIAS Deadeye_deadeyed)
set_target_properties(Deadeye_deadeyed PROPERTIES
  OUTPUT_NAME deadeyed
  EXPORT_NAME deadeyed
)

target_link_libraries(Deadeye_deadeyed
    PRIVATE
    Deadeye_library
)
