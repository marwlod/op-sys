cmake_minimum_required(VERSION 3.15)
project(op-sys C)

set(CMAKE_C_STANDARD 11)

add_executable(lab1-ex2 sort_library.c static_check.c shared_check.c dynamic_check.c)