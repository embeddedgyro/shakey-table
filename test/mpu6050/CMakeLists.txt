# Add the executable
add_executable(mpu6050_AccelOffset_ut mpu6050_AccelOffset_ut.cpp)
add_executable(mpu6050_AccelRaw_ut mpu6050_AccelRaw_ut.cpp) 
add_executable(mpu6050_GyroOffset_ut mpu6050_GyroOffset_ut.cpp)
                        
add_executable(mpu6050_GyroRaw_ut mpu6050_GyroRaw_ut.cpp )
add_executable(mpu6050_GyroSampleRateDivider_ut mpu6050_GyroSampleRateDivider_ut.cpp)
add_executable(mpu6050_SensorFIFOConfig_ut mpu6050_SensorFIFOConfig_ut.cpp)
                        
add_executable(mpu6050_SensorFIFOEnable_ut mpu6050_SensorFIFOEnable_ut.cpp )
add_executable(mpu6050_SensorIntrPinConfig_ut mpu6050_SensorIntrPinConfig_ut.cpp )
add_executable(mpu6050_Wakeup_ut mpu6050_Wakeup_ut.cpp)


# Link the libraries
target_link_libraries(mpu6050_AccelOffset_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_AccelRaw_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_GyroOffset_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_GyroRaw_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_GyroSampleRateDivider_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_SensorFIFOConfig_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_SensorFIFOEnable_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_SensorIntrPinConfig_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_Wakeup_ut PUBLIC mpu6050 -lgpiodcxx)

# Specify include directories
target_include_directories(
  mpu6050_AccelOffset_ut
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/mpu6050"
         )