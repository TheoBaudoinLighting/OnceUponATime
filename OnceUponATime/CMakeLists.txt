cmake_minimum_required(VERSION 3.30)
project(OnceUponATime)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(examples)

add_executable(OnceUponATime src/main.cpp
        src/lexer.cpp
        src/lexer.hpp
        src/parser.cpp
        src/parser.hpp
        include/token.hpp
        src/code_generator.cpp
        src/code_generator.hpp
)
