cmake_minimum_required(VERSION 4.1)
project(can_network C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(can_network
        src/main.c
        src/io/can.c
        src/io/obd.c
        src/io/obd.h)
