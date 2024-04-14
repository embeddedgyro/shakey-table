# Create a library mpu6050 from the specified sources
add_library(mpu6050 mpu6050.cpp)
target_link_libraries(mpu6050 smbus_i2c_if)

target_include_directories(mpu6050 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ../i2c_interface)
