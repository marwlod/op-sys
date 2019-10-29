cmake_minimum_required(VERSION 3.15)
project(sys-op C)

set(CMAKE_C_STANDARD 11)

add_executable(sys-op ex1/program.c ex2/search_nftw.c ex2/search_stat.c)