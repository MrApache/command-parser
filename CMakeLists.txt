cmake_minimum_required(VERSION 3.30)
project(command_parser C)

set(CMAKE_C_STANDARD 11)
add_definitions(-DMEMPRNT)

add_library(command_parser SHARED
        src/lexer.c
        src/types.c
        src/parser.c
        src/errors.c
        src/allocator.c
        src/membuf.c
)
