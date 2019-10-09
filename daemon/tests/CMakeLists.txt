add_executable(Deadeye_tests
  main.cpp
  json.cpp
  link_config_test.cpp
  pipeline_config_test.cpp
  stream_config_test.cpp
)

target_link_libraries(Deadeye_tests
  Deadeye::library
)

include(Catch)
catch_discover_tests(Deadeye_tests)
