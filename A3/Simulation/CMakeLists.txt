cmake_minimum_required(VERSION 3.22)
project(Simulation)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_EXE_LINKER_FLAGS  "-rdynamic -ldl")
add_executable(Simulation main.cpp AlgorithmRegistrar.cpp)

# DO NOT TOUCH
set(COMMON_INCLUDES 
    ${PROJECT_SOURCE_DIR}/Common/common_types.h
    ${PROJECT_SOURCE_DIR}/Common/AbstractAlgorithm.h
    ${PROJECT_SOURCE_DIR}/Common/BatteryMeter.h
    ${PROJECT_SOURCE_DIR}/Common/DirtSensor.h
    ${PROJECT_SOURCE_DIR}/Common/WallsSensor.h)


set(SIM_INCLUDES
    ${PROJECT_SOURCE_DIR}/Simulation/BatteryMeterImpl.h
    ${PROJECT_SOURCE_DIR}/Simulation/WallsSensorImpl.h
    ${PROJECT_SOURCE_DIR}/Simulation/DirtSensorImpl.h
    ${PROJECT_SOURCE_DIR}/Simulation/House.h
    ${PROJECT_SOURCE_DIR}/Simulation/Utils.h
    ${PROJECT_SOURCE_DIR}/Simulation/Simulator.h
    ${PROJECT_SOURCE_DIR}/Simulation/RobotState.h
    )

set(ALGO_INCLUDES
    ${PROJECT_SOURCE_DIR}/Algorithm/MyAlgorithm.h
    ${PROJECT_SOURCE_DIR}/Algorithm/types.h
    ${PROJECT_SOURCE_DIR}/Algorithm/Utils.h
    ${PROJECT_SOURCE_DIR}/Algorithm/HouseManager.h
    )

add_library(sim SHARED 
    ${PROJECT_SOURCE_DIR}/Simulation/BatteryMeterImpl.cpp
    ${PROJECT_SOURCE_DIR}/Simulation/WallsSensorImpl.cpp
    ${PROJECT_SOURCE_DIR}/Simulation/DirtSensorImpl.cpp
    ${PROJECT_SOURCE_DIR}/Simulation/House.cpp
    ${PROJECT_SOURCE_DIR}/Simulation/Simulator.cpp
    ${PROJECT_SOURCE_DIR}/Simulation/RobotState.cpp
    ${PROJECT_SOURCE_DIR}/Simulation/Utils.cpp
    )
target_include_directories(sim PUBLIC ${COMMON_INCLUDES} ${SIM_INCLUDES})

add_executable( ${PROJECT_NAME} main.cpp 
    ${PROJECT_SOURCE_DIR}/Algorithm/MyAlgorithm.cpp
    ${PROJECT_SOURCE_DIR}/Algorithm/HouseManager.cpp
    ${PROJECT_SOURCE_DIR}/Algorithm/Utils.cpp
    )
target_include_directories( ${PROJECT_NAME} 
    PUBLIC ${COMMON_INCLUDES} ${SIM_INCLUDES} ${ALGO_INCLUDES})

target_link_libraries( ${PROJECT_NAME} PUBLIC sim )
