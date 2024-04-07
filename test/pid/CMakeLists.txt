# Add the executable
add_executable(pidUnitTest pidUnitTest.cpp)

# Link the libraries
target_link_libraries((pidUnitTest PUBLIC pid)

# Specify include directories
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/pid")