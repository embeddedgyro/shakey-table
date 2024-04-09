# Add the executable
add_executable(MotorDriver_Test Motordriver_DutyCycle_ut.cpp)

# Link the libraries
target_link_libraries(MotorDriver_Test PUBLIC MotorDriver)

# Specify include directories
target_include_directories(
    MotorDriver_Test 
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/MotorDriver")