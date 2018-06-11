cmake_minimum_required(VERSION 3.9)
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_RANLIB avr-ranlib)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(include)
add_library(avr-hd44780 src/hd44780.c include/hd44780.h)