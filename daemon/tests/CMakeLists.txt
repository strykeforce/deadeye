add_executable(Deadeye_tests
  main.cpp
)

add_subdirectory(config)
add_subdirectory(link)

if(DEADEYE_BUILD_BENCHMARKS)
  target_compile_definitions(Deadeye_tests
    PRIVATE
    CATCH_CONFIG_ENABLE_BENCHMARKING
  )
endif(DEADEYE_BUILD_BENCHMARKS)

target_link_libraries(Deadeye_tests
  Deadeye::library
)

include(Catch)
catch_discover_tests(Deadeye_tests)
