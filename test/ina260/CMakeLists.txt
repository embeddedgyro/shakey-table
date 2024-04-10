# Add the executable
add_executable(ina260_Wakeup_ut ina260_Wakeup_ut.cpp)
add_executable(ina260_ReadCurrent_ut ina260_ReadCurrent_ut.cpp)
add_executable(ina260_ReadPower_ut ina260_ReadPower_ut.cpp)
add_executable(ina260_ReadVoltage_ut ina260_ReadVoltage.cpp)

# Link the libraries
target_link_libraries(ina260_Wakeup_ut PUBLIC ina260 -lgpiodcxx)
target_link_libraries(ina260_ReadCurrent_ut PUBLIC ina260 -lgpiodcxx)
target_link_libraries(ina260_ReadPower_ut PUBLIC ina260 -lgpiodcxx)
target_link_libraries(ina260_ReadVoltage_ut PUBLIC ina260 -lgpiodcxx)

# Specify include directories
target_include_directories(
  ina260_Wakeup_ut 
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/ina260"
         )