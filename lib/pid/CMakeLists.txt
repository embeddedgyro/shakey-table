# Create a library pid from the specified sources
add_library(pid pid.cpp)

target_include_directories(pid PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
