cmake_minimum_required(VERSION 3.1...3.29)

project(chisaka LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Change path from /src if needed, or add more directories

include_directories(./../kernel/src)

set(sources main.cpp)

add_executable(main ${sources})

target_compile_options(main PRIVATE 
        -std=c++20 
        -Wall 
        -Wextra 
        -Wshadow 
        -Wpointer-arith 
        -Wcast-align
        -Wswitch-enum 
)
