cmake_minimum_required(VERSION 3.0)
project(CPU6502)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++17")
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

find_package (Threads)
find_package(SFML COMPONENTS graphics window system REQUIRED)
add_executable(CPU6502 ${SOURCES})
target_link_libraries(CPU6502 ${CMAKE_THREAD_LIBS_INIT} sfml-graphics sfml-window sfml-system)
