add_library(tinyfsm INTERFACE)
target_include_directories(tinyfsm
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/tinyfsm/include/
)

add_library(hedley INTERFACE)
target_include_directories(hedley
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/hedley/
)
