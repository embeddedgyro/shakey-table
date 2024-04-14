# Create a library mpu6050 from the specified sources
add_library(MotorDriver MotorDriver.cpp)

target_include_directories(MotorDriver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
