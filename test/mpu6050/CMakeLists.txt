# Add the executable
add_executable(MPU_Test mpu6050_AccelOffset_ut.cpp mpu6050_AccelRaw_ut.cpp mpu6050_GyroOffset_ut.cpp
                        mpu6050_GyroRaw_ut.cpp mpu6050_GyroSampleRateDivider_ut.cpp mpu6050_SensorFIFOConfig_ut.cpp
                        mpu6050_SensorFIFOEnable_ut.cpp mpu6050_SensorIntrPinConfig_ut.cpp mpu6050_Wakeup_ut.cpp)


# Link the libraries
target_link_libraries(MPU_Test PUBLIC mpu6050 -lgpiodcxx)

# Specify include directories
target_include_directories(
  MPU_Test
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/mpu6050"
         )