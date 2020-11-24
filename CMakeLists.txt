cmake_minimum_required(VERSION 3.15)
project(learnopengl)

set(CMAKE_CXX_STANDARD 14)

if (MACOS)
	link_directories(/usr/local/lib)
	include_directories(/usr/local/include)
elseif(WIN32)
	link_directories(F:\\vcpkg\\installed\\x64-windows\\lib C:\\glfw-3.3.2.bin.WIN64\\lib-vc2019)
	include_directories(F:\\vcpkg\\installed\\x64-windows\\include)
endif()

add_executable(learnopengl main.cpp glad.c)

if (MACOS)
	target_link_libraries(learnopengl glfw)
elseif(WIN32)
	target_link_libraries(learnopengl glfw3)
endif()
