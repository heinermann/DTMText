cmake_minimum_required (VERSION 2.8)
project (DTMtxt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable (dtm2txt dtm2txt.cpp)
add_executable (txt2dtm txt2dtm.cpp)

