cmake_minimum_required(VERSION 3.14)
project(client)

set(CMAKE_CXX_STANDARD 14)

add_executable(client main.cpp day02/tcp_client.cpp day02/tcp_client.h)