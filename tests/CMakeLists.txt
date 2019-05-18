add_executable(tests
  main.cpp
  json.cpp
  pipeline_config_test.cpp
)

target_link_libraries(tests
  Catch2::Catch2
  nlohmann_json::nlohmann_json
  deadeye
)

include(Catch)
catch_discover_tests(tests)
