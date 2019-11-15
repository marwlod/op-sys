cmake_minimum_required(VERSION 3.15)
project(op-sys C)
set(CMAKE_C_STANDARD 11)

add_executable(lab1-compiler_optimization fib.c insertion.c string_to_list.c mallocs.c)