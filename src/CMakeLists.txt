# Add the executable
add_executable(${PROJECT_NAME} main.cpp)
add_executable(mpu_testing mpu_testing.cpp)
add_executable(ina_testing ina_testing.cpp)

# Link the libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ina260 mpu6050 pid MotorDriver -lgpiodcxx)
target_link_libraries(mpu_testing PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(ina_testing PUBLIC ina260 -lgpiodcxx)

# Specify include directories
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/ina260" "${PROJECT_SOURCE_DIR}/lib/mpu6050"
         "${PROJECT_SOURCE_DIR}/lib/pid"
         "${PROJECT_SOURCE_DIR}/lib/MotorDriver")
