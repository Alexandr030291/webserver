cmake_minimum_required(VERSION 3.6)
project(webserver)

set(CMAKE_CXX_STANDARD 14)


link_libraries(-lpthread)

set(SOURCE_FILES
        filemanager.h
        filemanager.cpp
        http.h
        http.cpp
        client.h
        client.cpp
        epoll.h
        epoll.cpp
        server.h
        server.cpp
        main.cpp )


add_executable(webserver ${SOURCE_FILES})