cmake_minimum_required(VERSION 3.15)
project(op-sys C)
set(CMAKE_C_STANDARD 11)

add_executable(lab2-search_dir search_nftw.c search_stat.c)