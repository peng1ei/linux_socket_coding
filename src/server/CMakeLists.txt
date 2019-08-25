cmake_minimum_required(VERSION 3.14)
project(server)

set(CMAKE_CXX_STANDARD 14)

add_executable(server
        main.cpp
        day01/tcp_server.cpp
        day01/tcp_server.h
        day02/process_server.cpp
        day02/process_server.h
        core/warp/warp_socket.h
        core/warp/warp_socket.cpp day02/pthread_server.cpp day02/pthread_server.h)

find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)