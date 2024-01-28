cmake_minimum_required(VERSION 3.10)

project(VCUUnitTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-O1")

file(GLOB SOURCES
    ../UserLib/SensorInterfaceLib/*.cpp
    ../UserLib/MCUInterfaceLib/*.cpp
    ../UserLib/DataStoreLib/*.cpp
    ../UserLib/UtlisLib/*.cpp
)

add_executable(unit-tests test.cpp ${SOURCES})

target_compile_definitions(unit-tests PUBLIC MOCK_TEST=1)
target_include_directories(unit-tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/doctest")
target_include_directories(unit-tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Inc")
target_include_directories(unit-tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../UserLib")
