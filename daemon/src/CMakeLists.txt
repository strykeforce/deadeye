# Build library to facilitate testing
configure_file(config/deadeye_version.cpp.in deadeye_version.cpp @ONLY)

add_library(Deadeye_library
    controller.cpp
    log.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/deadeye_version.cpp
)

add_subdirectory(capture)
add_subdirectory(config)
add_subdirectory(hardware)
add_subdirectory(link)
add_subdirectory(log)
add_subdirectory(pipeline)
add_library(Deadeye::library ALIAS Deadeye_library)
set_target_properties(Deadeye_library PROPERTIES
  OUTPUT_NAME deadeye
  EXPORT_NAME deadeye
)

if(CMAKE_CROSSCOMPILING)
  link_directories(${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu/tegra/)
  message(
    STATUS
    "Cross-compiling for ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR} on ${CMAKE_HOST_SYSTEM_NAME}"
  )
endif(CMAKE_CROSSCOMPILING)

target_compile_definitions(Deadeye_library
  PUBLIC
  DEADEYE_UNIT_ID=${DEADEYE_UNIT_ID}
  DEADEYE_NT_SERVER=${DEADEYE_NT_SERVER}
  DEADEYE_CLIENT_ADDRESS=${DEADEYE_CLIENT_ADDRESS}
  DEADEYE_CLIENT_PORT=${DEADEYE_CLIENT_PORT}
)


target_include_directories(Deadeye_library
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(Deadeye_library
    PUBLIC
    ntcore
    cscore
    nlohmann_json::nlohmann_json
    tinyfsm
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_link_libraries(Deadeye_library PUBLIC spdlog::spdlog_header_only)
else()
  target_link_libraries(Deadeye_library PUBLIC spdlog::spdlog)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
target_link_libraries(Deadeye_library
    PRIVATE
    PkgConfig::SYSTEMD
)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
target_link_libraries(Deadeye_library
    PRIVATE
    PkgConfig::GPIOD
)
endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")


# Build main target
if(enableBuildMain)

add_executable(Deadeye_deadeyed
    main.cpp
)
add_executable(Deadeye::deadeyed ALIAS Deadeye_deadeyed)
set_target_properties(Deadeye_deadeyed PROPERTIES
  OUTPUT_NAME deadeyed
  EXPORT_NAME deadeyed
)

target_compile_definitions(Deadeye_deadeyed
  PUBLIC
  $<$<BOOL:${DEADEYE_CAMERA0_PIPELINE}>:DEADEYE_CAMERA0_PIPELINE=${DEADEYE_CAMERA0_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA1_PIPELINE}>:DEADEYE_CAMERA1_PIPELINE=${DEADEYE_CAMERA1_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA2_PIPELINE}>:DEADEYE_CAMERA2_PIPELINE=${DEADEYE_CAMERA2_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA3_PIPELINE}>:DEADEYE_CAMERA3_PIPELINE=${DEADEYE_CAMERA3_PIPELINE}>
  $<$<BOOL:${DEADEYE_CAMERA4_PIPELINE}>:DEADEYE_CAMERA4_PIPELINE=${DEADEYE_CAMERA4_PIPELINE}>
)


target_link_libraries(Deadeye_deadeyed
    PRIVATE
    Deadeye_library
)

include(GNUInstallDirs)
install(TARGETS Deadeye_deadeyed DESTINATION ${CMAKE_INSTALL_BINDIR})

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
add_executable(Deadeye_shutdownd
  shutdown.cpp
  log.cpp
)
add_executable(Deadeye::shutdownd ALIAS Deadeye_shutdownd)
set_target_properties(Deadeye_shutdownd PROPERTIES
  OUTPUT_NAME shutdownd
  EXPORT_NAME shutdownd
)

target_link_libraries(Deadeye_shutdownd
  PRIVATE
  spdlog::spdlog
  PkgConfig::GPIOD
  PkgConfig::SYSTEMD
)

install(TARGETS Deadeye_shutdownd DESTINATION ${CMAKE_INSTALL_BINDIR})

endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")


endif(enableBuildMain)
