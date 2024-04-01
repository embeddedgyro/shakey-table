# Create a library ina260 from the specified sources
add_library(ina260 ina260.cpp)
target_link_libraries(ina260 smbus_i2c_if)

target_include_directories(ina260 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ../i2c_interface)
