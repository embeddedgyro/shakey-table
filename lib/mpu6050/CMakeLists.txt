# Create a library mpu6050 from the specified sources
add_library(mpu6050 i2c_interface.cpp mpu6050.cpp smbus_i2c_if.cpp)

target_include_directories(mpu6050 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
