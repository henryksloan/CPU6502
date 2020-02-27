cmake_minimum_required(VERSION 3.0)
project(CPU6502)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++17")
include_directories(include)
file(GLOB TEST_SOURCES "src/*.cpp")
set(LIB_SOURCES src/cpu_6502.cpp src/disassembler.cpp src/instruction.cpp)

find_package (Threads)
find_package(SFML COMPONENTS graphics window system REQUIRED)
add_executable(CPU6502_test ${TEST_SOURCES})
add_library(CPU6502 STATIC ${LIB_SOURCES})
target_link_libraries(CPU6502_test ${CMAKE_THREAD_LIBS_INIT} sfml-graphics sfml-window sfml-system)
