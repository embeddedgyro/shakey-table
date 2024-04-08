<<<<<<< HEAD
find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories (${PROJECT_SOURCE_DIR}/src
			${Boost_INCLUDE_DIRES}
			)
add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable (Test test.cpp)
target_link_libraries(Test PUBLIC ina260 mpu6050 pid MotorDriver -lgpiodcxx
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test (NAME MyTest COMMAND Test)			
=======
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
>>>>>>> 33da97b7a9825819573edaba8463f68d19e8716c
