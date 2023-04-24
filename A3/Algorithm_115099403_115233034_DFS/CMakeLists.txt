cmake_minimum_required(VERSION 3.22)

project(Algorithm_115099403_115233034_DFS)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_SHARED_LINKER_FLAGS "-fPIC -shared -fvisibility=hidden -fvisibility-inlines-hidden")

# DO NOT TOUCH
set(COMMON_INCLUDES 
    ${PROJECT_SOURCE_DIR}/Common/common_types.h
    ${PROJECT_SOURCE_DIR}/Common/AbstractAlgorithm.h
    ${PROJECT_SOURCE_DIR}/Common/BatteryMeter.h
    ${PROJECT_SOURCE_DIR}/Common/DirtSensor.h
    ${PROJECT_SOURCE_DIR}/Common/WallsSensor.h)

set(ALGO_COMMON_INCLUDES
    ${PROJECT_SOURCE_DIR}/AlgorithmCommon/AlgorithmRegistrar.h
    )

set(ALGO_INCLUDES
    ${PROJECT_SOURCE_DIR}/Algorithm/include/Algorithm_115099403_115233034_DFS.h
    ${PROJECT_SOURCE_DIR}/Algorithm/include/types.h
    ${PROJECT_SOURCE_DIR}/Algorithm/include/Utils.h
    ${PROJECT_SOURCE_DIR}/Algorithm/include/HouseManager.h
    )

add_library(${PROJECT_NAME} SHARED 
    ${PROJECT_SOURCE_DIR}/src/Algorithm_115099403_115233034_DFS.cpp
    ${PROJECT_SOURCE_DIR}/src/HouseManager.cpp
    ${PROJECT_SOURCE_DIR}/src/Utils.cpp
    )

target_include_directories(${PROJECT_NAME} PUBLIC
    # ${ALGO_COMMON_INCLUDES} 
    ${COMMON_INCLUDES}
    ${ALGO_INCLUDES}
    )

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/algo_dir/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/libAlgorithm_115099403_115233034_DFS.so ${CMAKE_BINARY_DIR}/algo_dir/)
