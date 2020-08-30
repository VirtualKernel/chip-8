cmake_minimum_required(VERSION 3.14)
project(interpreter)

set(CMAKE_CXX_STANDARD 11)

find_package(SDL2 REQUIRED)

add_executable(
	interpreter
	src/interpreter.cpp
	src/main.cpp
	src/video.cpp)

target_compile_options(interpreter PRIVATE -Wall)

target_link_libraries(interpreter PRIVATE SDL2::SDL2)
