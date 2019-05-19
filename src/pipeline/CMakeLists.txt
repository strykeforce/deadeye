target_sources(Deadeye_library
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/pipeline_config.cpp
  ${CMAKE_CURRENT_LIST_DIR}/default_pipeline.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_pattern_pipeline.cpp
)

target_include_directories(Deadeye_library
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)
