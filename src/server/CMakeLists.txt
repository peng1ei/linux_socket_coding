cmake_minimum_required(VERSION 3.14)
project(server)

set(CMAKE_CXX_STANDARD 14)

add_executable(server main.cpp day01/tcp_server.cpp day01/tcp_server.h)