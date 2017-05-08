cmake_minimum_required(VERSION 3.6)
project(webserver)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp filemanager.h epoll.cpp epoll.h http.h client.cpp client.h classes.h server.cpp server.h)

link_libraries(-lpthread)

add_executable(webserver ${SOURCE_FILES})