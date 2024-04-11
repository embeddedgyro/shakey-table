cmake_minimum_required(VERSION 3.10)
# Project name and version
project(ShakeyTable VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include subdirectories
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)
enable_testing()

# Define a helper function to run multiple tests as subtests
function(add_multiple_subtests TEST_NAME)
    set(TEST_NAMES "${ARGN}")
    foreach(TEST ${TEST_NAMES})
        add_test(NAME "${TEST_NAME}_${TEST}" COMMAND ${TEST})
    endforeach()
endfunction()


add_multiple_subtests(online
        mpu6050_AccelOffset_ut
        mpu6050_AccelRaw_ut
        mpu6050_GyroOffset_ut
        mpu6050_GyroRaw_ut
        mpu6050_GyroSampleRateDivider_ut
        mpu6050_SensorFIFOConfig_ut
        mpu6050_SensorFIFOEnable_ut
        mpu6050_SensorIntrPinConfig_ut
        mpu6050_Wakeup_ut
        ina260_Wakeup_ut
        ina260_ReadCurrent_ut
        ina260_ReadPower_ut
        ina260_ReadVoltage_ut
)

add_multiple_subtests(offline
        MotorDriver_Test
        PID_Test
)

add_test(NAME On_Test COMMAND online)
add_test(NAME Off_Test COMMAND offline)

# Generate Doxyfile and associated target
find_package(Doxygen)
set(DOXYGEN_GENERATE_LATEX YES)
doxygen_add_docs(
    doc
    ${PROJECT_SOURCE_DIR}
)

# Define multiple subtests under the "mpu" test case
#add_multiple_subtests(mpu
#   mpu6050_AccelOffset_ut
#    mpu6050_AccelRaw_ut
#    mpu6050_GyroOffset_ut
#    mpu6050_GyroRaw_ut
#    mpu6050_GyroSampleRateDivider_ut
#    mpu6050_SensorFIFOConfig_ut
#    mpu6050_SensorFIFOEnable_ut
#    mpu6050_SensorIntrPinConfig_ut
#   mpu6050_Wakeup_ut
#)

# Define multiple subtests under the "ina" test case
#add_multiple_subtests(ina
#        ina260_Wakeup_ut
#        ina260_ReadCurrent_ut
#        ina260_ReadPower_ut
#        ina260_ReadVoltage_ut
#)

#add_test(NAME mpu COMMAND mpu6050_AccelOffset_ut mpu6050_AccelRaw_ut mpu6050_GyroOffset_ut mpu6050_GyroRaw_ut mpu6050_GyroSampleRateDivider_ut mpu6050_SensorFIFOConfig_ut mpu6050_SensorFIFOEnable_ut mpu6050_SensorIntrPinConfig_ut mpu6050_Wakeup_ut)
#add_test(NAME ina260 COMMAND ina260_Wakeup_ut ina260_ReadCurrent_ut ina260_ReadPower_ut ina260_ReadVoltage_ut)
#add_test(NAME motordriver COMMAND MotorDriver_Test)
#add_test(NAME pid COMMAND PID_Test)




