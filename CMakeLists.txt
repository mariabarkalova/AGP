cmake_minimum_required(VERSION 3.30.3)

set(PROJECT_NAME_STR Piyavsky-method)
set(CMAKE_CXX_STANDARD 17)

project (${PROJECT_NAME_STR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/ ${CMAKE_CURRENT_SOURCE_DIR}/include/ ${CMAKE_CURRENT_SOURCE_DIR}/libs/sample_src ${CMAKE_CURRENT_SOURCE_DIR}/libs/src)

file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/libs/sample_src/Hill/*.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/sample_src/Shekel/*.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/src/*.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/src/*.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/libs/src/*.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME_STR} ${SRC_FILES})
