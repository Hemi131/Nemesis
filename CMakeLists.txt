cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Nemesis)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -ansi -pedantic")

# Specify the C standard
# set(CMAKE_C_STANDARD 89)
# set(CMAKE_C_STANDARD_REQUIRED True)

# Add the source files
set(SOURCES
    src/main.c
    src/matrix.c
    src/stack.c
    src/parser.c
    src/simplex.c
    src/queue.c
)

# Add the executable
add_executable(stack_program ${SOURCES})