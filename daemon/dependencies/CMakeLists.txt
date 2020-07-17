# Add tinyfsm dependency
add_library(tinyfsm INTERFACE)
target_include_directories(tinyfsm
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/tinyfsm/include/
)

# Add readerwriterqueue dependency
add_library(readerwriterqueue INTERFACE)
target_include_directories(readerwriterqueue
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/readerwriterqueue/
)
