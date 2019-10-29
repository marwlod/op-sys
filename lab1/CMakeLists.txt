cmake_minimum_required(VERSION 3.15)
project(sys-op C)

set(CMAKE_C_STANDARD 11)

add_executable(sys-op ex1/fib.c ex1/insertion.c ex1/mallocs.c ex1/string_to_list.c ex2/sort_library.c
        ex2/static_check.c ex2/dynamic_check.c ex2/shared_check.c)