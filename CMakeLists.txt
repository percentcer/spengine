cmake_minimum_required(VERSION 3.15)
project(learnopengl)

set(CMAKE_CXX_STANDARD 14)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

add_executable(learnopengl main.cpp glad.c)

target_link_libraries(learnopengl glfw)
