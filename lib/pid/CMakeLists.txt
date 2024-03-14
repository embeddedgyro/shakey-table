project(pidlib)

add_library(${PROJECT_NAME} pid.cpp)
add_library(pid_control ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})



