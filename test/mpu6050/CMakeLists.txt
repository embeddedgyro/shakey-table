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
