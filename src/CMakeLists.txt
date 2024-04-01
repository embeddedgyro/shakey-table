# Add the executable
add_executable(${PROJECT_NAME} main.cpp)

# Link the libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ina260 mpu6050 pid MotorDriver)

# Specify include directories
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/ina260" "${PROJECT_SOURCE_DIR}/lib/mpu6050"
         "${PROJECT_SOURCE_DIR}/lib/pid"
         "${PROJECT_SOURCE_DIR}/lib/MotorDriver")
