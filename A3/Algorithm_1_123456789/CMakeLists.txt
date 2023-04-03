cmake_minimum_required(VERSION 3.22)

project(Algorithm_1_123456789)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_SHARED_LINKER_FLAGS "-fPIC -shared -fvisibility=hidden -fvisibility-inlines-hidden")
add_library(Algorithm_1_123456789 SHARED Algo_123456789.cpp)
target_include_directories(Algorithm_1_123456789 PUBLIC include)