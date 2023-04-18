cmake_minimum_required(VERSION 3.22)
project(Simulation)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_EXE_LINKER_FLAGS  "-rdynamic -ldl")

set(SIM_INCLUDES
    ${PROJECT_SOURCE_DIR}/include/InputParser.h
    ${PROJECT_SOURCE_DIR}/include/Utils.h
    ${PROJECT_SOURCE_DIR}/include/RobotState.h
    ${PROJECT_SOURCE_DIR}/include/BatteryMeterImpl.h
    ${PROJECT_SOURCE_DIR}/include/WallsSensorImpl.h
    ${PROJECT_SOURCE_DIR}/include/DirtSensorImpl.h
    ${PROJECT_SOURCE_DIR}/include/House.h
    ${PROJECT_SOURCE_DIR}/include/Simulator.h
    ${PROJECT_SOURCE_DIR}/include/Config.h
    )

# add_library(${PROJECT_NAME} SHARED 
#     ${PROJECT_SOURCE_DIR}/src/BatteryMeterImpl.cpp
#     ${PROJECT_SOURCE_DIR}/src/WallsSensorImpl.cpp
#     ${PROJECT_SOURCE_DIR}/src/DirtSensorImpl.cpp
#     ${PROJECT_SOURCE_DIR}/src/House.cpp
#     ${PROJECT_SOURCE_DIR}/src/Simulator.cpp
#     ${PROJECT_SOURCE_DIR}/src/RobotState.cpp
#     ${PROJECT_SOURCE_DIR}/src/Utils.cpp
#     ${PROJECT_SOURCE_DIR}/AlgorithmRegistrar.cpp
#     )
    
# target_include_directories(${PROJECT_NAME} PUBLIC ${COMMON_INCLUDES} ${SIM_INCLUDES})

add_executable( ${PROJECT_NAME} main.cpp 
    ${PROJECT_SOURCE_DIR}/src/BatteryMeterImpl.cpp
    ${PROJECT_SOURCE_DIR}/src/WallsSensorImpl.cpp
    ${PROJECT_SOURCE_DIR}/src/DirtSensorImpl.cpp
    ${PROJECT_SOURCE_DIR}/src/House.cpp
    ${PROJECT_SOURCE_DIR}/src/Simulator.cpp
    ${PROJECT_SOURCE_DIR}/src/RobotState.cpp
    ${PROJECT_SOURCE_DIR}/src/Utils.cpp
    ${PROJECT_SOURCE_DIR}/src/InputParser.cpp
    ${PROJECT_SOURCE_DIR}/AlgorithmRegistrar.cpp
    )
target_include_directories( ${PROJECT_NAME} 
    PUBLIC 
    # ${COMMON_INCLUDES} 
    ${SIM_INCLUDES} 
    # ${ALGO_INCLUDES}
    )

target_link_libraries( ${PROJECT_NAME} PRIVATE ${common_libs})


# add_executable(Simulation main.cpp AlgorithmRegistrar.cpp)
# target_include_directories(Simulation PRIVATE 
#     ${COMMON_INCLUDES} 
#     ${SIM_INCLUDES} 
#     ${ALGO_INCLUDES}
#     ${CMAKE_CURRENT_SOURCE_DIR}/../algo1_lib
#     ${CMAKE_CURRENT_SOURCE_DIR}/../algo2_lib)
# target_link_libraries(Simulation PRIVATE ${common_libs})
