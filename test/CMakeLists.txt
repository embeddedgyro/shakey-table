# Add subdirectories for different test modules
#add_subdirectory(ina260)
add_subdirectory(mpu6050)
add_subdirectory(pid)
#add_subdirectory(motordriver)

# Aggregate all test executables into a single target

#add_custom_target(OnlineTest
#    DEPENDS MPU_Test INA_Test
#    COMMENT "Running Online tests"
#)

#add_custom_target(OfflineTest
#    DEPENDS PID_Test MotorDriver_Test
#    COMMENT "Running Offline tests"
#)

