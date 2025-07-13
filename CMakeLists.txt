cmake_minimum_required(VERSION 3.31)
project(todoList)

set(CMAKE_CXX_STANDARD 20)

set(JSON_INCLUDE_DIR D:/cppLiba/json/single_include)

include_directories(${JSON_INCLUDE_DIR})

add_executable(todoList main.cpp)
