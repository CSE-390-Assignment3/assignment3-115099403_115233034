cmake_minimum_required(VERSION 3.22)
project(Simulation)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_EXE_LINKER_FLAGS  "-rdynamic -ldl")
add_executable(Simulation main.cpp AlgorithmRegistrar.cpp)
target_include_directories(Simulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../algo1_lib
                                             ${CMAKE_CURRENT_SOURCE_DIR}/../algo2_lib)
target_link_libraries(Simulation PRIVATE ${common_libs})