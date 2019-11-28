cmake_minimum_required(VERSION 3.15)
project(op-sys C)
set(CMAKE_C_STANDARD 11)

add_subdirectory(batch_job_interpreter_pipes)
add_subdirectory(sigint_sigtstp)
add_subdirectory(sigusr)