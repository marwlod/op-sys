# Operating Systems course, fifth semester CS

This repository contains solutions to assignments for the Operating Systems course.

## Lab 1 - Memory management, libraries and time measurement
### Compiler optimisation ([code](lab1_mem_mgmt_and_libs/compiler_optimization))
*Summary*: Write four arbitrary short programs, time their execution using different compiler optimisation flags.

### Libraries ([code](lab1_mem_mgmt_and_libs/libraries))
*Summary*: Extract library from one of previous programs. Create executables using statically loaded, 
dynamically loaded and shared libraries. 


## Lab 2 - File system, operations on files
### Comparison of system and libraries IO functions ([code](lab2_file_system/system_vs_lib_io))
*Summary*: Write a program comparing performance of system and library IO functions. In order to do that, provide the API
functions: generate, sort, copy. Sort and copy functions need to be implemented in two variants: system and library.

### Performing operations on directories ([code](lab2_file_system/search_dir))
*Summary*: Write a program that searches the specified directory with all subdirectories for files that have a modification date 
less/equal/greater than specified.


## Lab 3 - Processes
### Performing operations on directories using one process per directory ([code](lab3_processes/search_dir_mult_ps))
*Summary*: Modify previous program, so it searches each directory in a separate process.

### Batch job interpreter ([code](lab3_processes/batch_job_interpreter))
*Summary*: Write a simple batch job interpreter that reads a file containing jobs (program name with arguments e.g. `ls -la`) in each line.


## Lab 4 - Signals and pipes
### SIGINT, SIGTSTP ([code](lab4_signals_and_pipes/sigint_sigtstp))
*Summary*: Write a program that has custom handlers defined for SIGINT and SIGTSTP signals. Program should create a subprocess
that prints date in an infinite loop and should pass above signals to this subprocess.

### SIGUSR1, SIGUSR2 ([code](lab4_signals_and_pipes/sigusr))
*Summary*: Write a program that creates a subprocess and sends n SIGUSR1 signals and then one SIGUSR2.
The subprocess should send back all the SIGUSR1 signals and after receiving SIGUSR2 terminate itself.

### Batch job interpreter handling pipe operator ([code](lab4_signals_and_pipes/batch_job_interpreter_pipes))
*Summary*: Modify program from previous lab, so it can handle multiple programs piped together, e.g. `ls -la | grep txt | awk '{print $1}'`