cmake_minimum_required(VERSION 3.28.3)

project(antiquake)

add_executable(${PROJECT_NAME} main.cpp)
add_subdirectory(lib/pid)

target_link_libraries(pidcontrol
)

target_link_directories(${PROJECT_NAME} PUBLIC lib/pid/)
