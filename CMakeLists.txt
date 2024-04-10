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

add_test(NAME mpu COMMAND mpu6050_AccelOffset_ut mpu6050_AccelRaw_ut mpu6050_GyroOffset_ut mpu6050_GyroRaw_ut mpu6050_GyroSampleRateDivider_ut mpu6050_SensorFIFOConfig_ut mpu6050_SensorFIFOEnable_ut mpu6050_SensorIntrPinConfig_ut mpu6050_Wakeup_ut)
add_test(NAME pid COMMAND PID_Test)

#add_test(NAME On_Test COMMAND Online_Test)
#add_test(NAME Off_Test COMMAND Offline_Test)

# Generate Doxyfile and associated target
find_package(Doxygen)
set(DOXYGEN_GENERATE_LATEX YES)
doxygen_add_docs(
    doc
    ${PROJECT_SOURCE_DIR}
)
