# Build library to facilitate testing
add_library(Deadeye_library
    controller.cpp
    camera.cpp
    lights.cpp
    link.cpp
)
add_subdirectory(config)
add_subdirectory(pipeline)
add_library(Deadeye::library ALIAS Deadeye_library)
set_target_properties(Deadeye_library PROPERTIES
  OUTPUT_NAME deadeye
  EXPORT_NAME deadeye
)

target_compile_definitions(Deadeye_library
  PUBLIC
  DEADEYE_UNIT_ID=${DEADEYE_UNIT_ID}
  $<$<BOOL:${DEADEYE_CAMERA0_PIPELINE}>:DEADEYE_CAMERA0_PIPELINE=${DEADEYE_CAMERA0_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA1_PIPELINE}>:DEADEYE_CAMERA1_PIPELINE=${DEADEYE_CAMERA1_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA2_PIPELINE}>:DEADEYE_CAMERA1_PIPELINE=${DEADEYE_CAMERA2_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA3_PIPELINE}>:DEADEYE_CAMERA1_PIPELINE=${DEADEYE_CAMERA3_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA4_PIPELINE}>:DEADEYE_CAMERA1_PIPELINE=${DEADEYE_CAMERA4_PIPELINE}>
  DEADEYE_NT_SERVER=${DEADEYE_NT_SERVER}
  DEADEYE_CLIENT_ADDRESS=${DEADEYE_CLIENT_ADDRESS}
  DEADEYE_CLIENT_PORT=${DEADEYE_CLIENT_PORT}
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
