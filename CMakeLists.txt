cmake_minimum_required(VERSION 3.5)
project(read_from_disk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ArrayFileReader.cpp ArrayFileReader.h
        ArrayStreamReader.cpp ArrayStreamReader.h
        BinaryFileParser.cpp BinaryFileParser.h
        Timer.h
        sarray2dfromfile.cpp sarray2dfromfile.h
        sparse_test.cpp)
add_executable(sparse_test.cpp ${SOURCE_FILES})