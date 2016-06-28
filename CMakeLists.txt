cmake_minimum_required(VERSION 3.5)
project(read_from_disk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ArrayFileReader.cpp ArrayFileReader.h BinaryFileParser.cpp BinaryFileParser.h Timer.h SparseFileParser.cpp SparseFileParser.h readFromDisk.cpp)
add_executable(read_from_disk ${SOURCE_FILES})