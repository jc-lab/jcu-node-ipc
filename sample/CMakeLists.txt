cmake_minimum_required(VERSION 3.8)
project(sample-app)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-D_WINSOCKAPI_)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} jcu-node-ipc)
