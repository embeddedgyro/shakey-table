# Add the executable
add_executable(mpu6050_ut mpu6050_ut.cpp)
add_executable(mpu6050_AccelOffset_ut mpu6050_AccelOffset_ut.cpp)

# Link the libraries
target_link_libraries(mpu6050_ut PUBLIC mpu6050 -lgpiodcxx)
target_link_libraries(mpu6050_AccelOffset_ut PUBLIC mpu6050 -lgpiodcxx)

# Specify include directories
target_include_directories(
  mpu6050_ut
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/mpu6050"
         )
target_include_directories(
  mpu6050_AccelOffset_ut
  PUBLIC "${PROJECT_SOURCE_DIR}/lib/mpu6050"
                 )