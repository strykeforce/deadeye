# Build library to facilitate testing
add_library(Deadeye_library
    controller.cpp
    camera.cpp
    lights.cpp
)
add_subdirectory(pipeline)
add_library(Deadeye::library ALIAS Deadeye_library)
set_target_properties(Deadeye_library PROPERTIES
  OUTPUT_NAME deadeye
  EXPORT_NAME deadeye
)

target_compile_definitions(Deadeye_library
  PUBLIC
  $<$<BOOL:${DEADEYE_CAMERA0_PIPELINE}>:DEADEYE_CAMERA0_PIPELINE=${DEADEYE_CAMERA0_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA1_PIPELINE}>:DEADEYE_CAMERA1_PIPELINE=${DEADEYE_CAMERA1_PIPELINE}>
)


target_include_directories(Deadeye_library
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

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
