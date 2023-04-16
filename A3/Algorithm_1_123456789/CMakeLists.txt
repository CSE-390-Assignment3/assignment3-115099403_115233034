cmake_minimum_required(VERSION 3.22)

project(Algorithm_1_123456789)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_SHARED_LINKER_FLAGS "-fPIC -shared -fvisibility=hidden -fvisibility-inlines-hidden")

add_library(Algorithm_1_123456789 SHARED src/Algo_123456789.cpp)
target_include_directories(Algorithm_1_123456789 PUBLIC include)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/algo_dir/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/libAlgorithm_1_123456789.so ${CMAKE_BINARY_DIR}/algo_dir/)
