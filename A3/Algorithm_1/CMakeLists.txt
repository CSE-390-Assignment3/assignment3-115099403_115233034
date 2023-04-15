cmake_minimum_required(VERSION 3.22)

project(Algorithm_1)

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
    ${PROJECT_SOURCE_DIR}/Algorithm/include/Algorithm_1.h
    ${PROJECT_SOURCE_DIR}/Algorithm/include/types.h
    ${PROJECT_SOURCE_DIR}/Algorithm/include/Utils.h
    ${PROJECT_SOURCE_DIR}/Algorithm/include/HouseManager.h
    )

add_library(Algorithm_1 SHARED 
    ${PROJECT_SOURCE_DIR}/src/Algorithm_1.cpp
    ${PROJECT_SOURCE_DIR}/src/HouseManager.cpp
    ${PROJECT_SOURCE_DIR}/src/Utils.cpp
    )

target_include_directories(Algorithm_1 PUBLIC
    # ${ALGO_COMMON_INCLUDES} 
    ${COMMON_INCLUDES}
    ${ALGO_INCLUDES}
    )
