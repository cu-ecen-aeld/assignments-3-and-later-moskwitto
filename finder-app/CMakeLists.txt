cmake_minimum_required(VERSION 3.13.4)

project(writer)

set(CMAKE_C_COMPILER  ${arm-cortex_a8-linux-gnueabihf-gcc})

add_executable(${PROJECT_NAME} writer.c)
