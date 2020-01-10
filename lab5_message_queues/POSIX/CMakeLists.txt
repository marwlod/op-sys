cmake_minimum_required(VERSION 3.15)
project(op-sys-lab5-posix C)
set(CMAKE_C_STANDARD 11)

add_executable(lab5-posix client.c server.c)
