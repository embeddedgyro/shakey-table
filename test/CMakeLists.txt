# Add subdirectories for different test modules
#add_subdirectory(ina260)
add_subdirectory(mpu6050)
add_subdirectory(pid)
#add_subdirectory(motordriver)

# Aggregate all test executables into a single target

add_custom_target(OnlineTest
    DEPENDS mpu6050_AccelOffset_ut mpu6050_AccelRaw_ut mpu6050_GyroOffset_ut mpu6050_GyroRaw_ut mpu6050_GyroSampleRateDivider_ut
    mpu6050_SensorFIFOConfig_ut mpu6050_SensorFIFOEnable_ut mpu6050_SensorIntrPinConfig_ut mpu6050_Wakeup_ut
    COMMENT "Running Online tests"
)

#add_custom_target(OfflineTest
#    DEPENDS PID_Test MotorDriver_Test
#    COMMENT "Running Offline tests"
#)

