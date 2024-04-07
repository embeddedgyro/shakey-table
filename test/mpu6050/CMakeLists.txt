# Add the executable
add_executable(mpu6050_AccelOffset_ut mpu6050_AccelOffset_ut.cpp)

# Link the libraries
target_link_libraries(mpu6050_AccelOffset_ut PUBLIC ina260 mpu6050 pid MotorDriver -lgpiodcxx)

# Specify include directories
target_include_directories(
  mpu6050_AccelOffset_ut
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/ina260" 
         "${PROJECT_SOURCE_DIR}/lib/mpu6050"
         "${PROJECT_SOURCE_DIR}/lib/pid"
         "${PROJECT_SOURCE_DIR}/lib/MotorDriver")