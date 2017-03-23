cmake_minimum_required(VERSION 3.6)
project(webserver)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(webserver ${SOURCE_FILES})