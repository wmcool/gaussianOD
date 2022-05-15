cmake_minimum_required(VERSION 3.5)
project(AnomalyDetectionCpp)
set(CMAKE_CXX_STANDARD 14)

add_executable(AnomalyDetectionCpp ${sourcefiles} ../src/main.cpp ../src/io.cpp ../src/detectanomalies.cpp ../src/statistics.cpp)