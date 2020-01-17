cmake_minimum_required(VERSION 3.15)
project(op-sys-lab6-systemv C)
set(CMAKE_C_STANDARD 11)

add_executable(lab6-systemv barber.c common.h client_spawner.c)
