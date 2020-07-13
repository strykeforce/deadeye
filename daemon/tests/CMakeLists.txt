add_executable(Deadeye_tests
  main.cpp
)

add_subdirectory(config)
add_subdirectory(link)
add_subdirectory(pipeline)

target_compile_definitions(Deadeye_tests
  PRIVATE
  $<$<BOOL:DEADEYE_BUILD_BENCHMARKS>:CATCH_CONFIG_ENABLE_BENCHMARKING>
  DEADEYE_TEST_DATA="${CMAKE_CURRENT_LIST_DIR}/data/"
)

target_link_libraries(Deadeye_tests
  Deadeye::library
)

include(Catch)
catch_discover_tests(Deadeye_tests)
