cmake_minimum_required(VERSION 3.22)
project(A3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(Algorithm_1_123456789)
add_subdirectory(Algorithm_2_123456789)
add_subdirectory(Simulation)

set(common_libs Algorithm_1_123456789 Algorithm_2_123456789)