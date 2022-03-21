target_sources(Deadeye_tests
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/filter_tests.cpp
        ${CMAKE_CURRENT_LIST_DIR}/filter_benchmarks.cpp
        ${CMAKE_CURRENT_LIST_DIR}/pipeline_benchmarks.cpp
        ${CMAKE_CURRENT_LIST_DIR}/pipeline_ops_tests.cpp
        ${CMAKE_CURRENT_LIST_DIR}/pipeline_ops_benchmarks.cpp
        ${CMAKE_CURRENT_LIST_DIR}/state_tests.cpp
        )
  
