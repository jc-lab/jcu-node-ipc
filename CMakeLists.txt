cmake_minimum_required(VERSION 3.8)
project(jcu-node-ipc CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(INC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/jcu/node_ipc/instance.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/jcu/node_ipc/ipc_config.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/jcu/node_ipc/session_attr.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/jcu/node_ipc/client.h
)

set(SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/trie_search.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/trie_search.h
)

add_library(${PROJECT_NAME} ${SRC_FILES} ${INC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)

find_package(uv REQUIRED)
target_link_libraries(${PROJECT_NAME} uv)

# find_package(uvw REQUIRED)
if(${UVW_INCLUDE_DIR})
    target_include_directories(${PROJECT_NAME} PRIVATE ${UVW_INCLUDE_DIR})
else()
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
    set(UVW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
endif()


add_subdirectory(jcu-transport)
add_subdirectory(sample)
add_subdirectory(jsoncpp)

# find_package(jsoncpp_lib REQUIRED)
target_link_libraries(${PROJECT_NAME} jsoncpp_lib)

# find_package(jcu-transport REQUIRED)
target_link_libraries(${PROJECT_NAME} jcu-transport)

