# Add the executable
add_executable(INA_Test ina260_Wakeup_ut ina260_ReadCurrent_ut.cpp ina260_ReadPower_ut.cpp ina260_ReadVoltage.cpp )


# Link the libraries
target_link_libraries(INA_Test PUBLIC ina260 -lgpiodcxx)

# Specify include directories
target_include_directories(
  INA_Test 
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/ina260"
         )