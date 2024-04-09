# Add the executable
add_executable(PID_Test pidUnitTest.cpp)

# Link the libraries
target_link_libraries(PID_Test PUBLIC pid)

# Specify include directories
target_include_directories(
  PID_Test 
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/pid")