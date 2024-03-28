# Add the executable
add_executable(MyExecutable main.cpp)

# Link the libraries
target_link_libraries(MyExecutable PUBLIC ina260 mpu6050 pid)

# Specify include directories
target_include_directories(
  MyExecutable
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/ina260" "${PROJECT_SOURCE_DIR}/lib/mpu6050"
         "${PROJECT_SOURCE_DIR}/lib/pid")
