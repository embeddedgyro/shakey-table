# Add the executable
add_executable(${PROJECT_NAME} pidUnitTest.cpp)

# Link the libraries
target_link_libraries(${PROJECT_NAME} PUBLIC pid)

# Specify include directories
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/pid")