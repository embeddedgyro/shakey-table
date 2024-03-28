# Create a library ina260 from the specified sources
add_library(ina260 i2c_interface.cpp ina260.cpp smbus_i2c_if.cpp)

target_include_directories(ina260 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
