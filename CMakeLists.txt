cmake_minimum_required(VERSION 3.17)
project(SAT C)

set(CMAKE_C_STANDARD 99)

add_executable(SAT main.c display.c head.h solver.c cnf_parser.c sudoku.c)