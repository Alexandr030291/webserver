cmake_minimum_required(VERSION 3.6)
project(webserver)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp
        filemanager.h
        http.h
        epoll.h
        epoll.cpp
        client.h
        client.cpp
        classes.h
        server.h
        server.cpp
        filemanager.cpp http.cpp)

link_libraries(-lpthread)

add_executable(webserver ${SOURCE_FILES})