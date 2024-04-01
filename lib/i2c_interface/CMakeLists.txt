# Create a library smbus_i2c_if
add_library(smbus_i2c_if smbus_i2c_if.cpp i2c_interface.cpp)
target_link_libraries(smbus_i2c_if -li2c)

target_include_directories(smbus_i2c_if PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
