cmake_minimum_required(VERSION 3.0)
project(CPU6502)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(CPU6502 ${SOURCES})
