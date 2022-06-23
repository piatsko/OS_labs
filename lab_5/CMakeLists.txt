cmake_minimum_required(VERSION 3.22)
project(lab_5)

set(CMAKE_CXX_STANDARD 98)

add_executable(Server Server.cpp employee.h)
add_executable(Client Client.cpp employee.h)
