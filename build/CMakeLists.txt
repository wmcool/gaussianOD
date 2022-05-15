project(AnomalyDetectionCpp)

file(GLOB sourcefiles *.cpp *.h)

set(EXECUTABLE_OUTPUT_PATH ../../bin)
set(LIBRARY_OUTPUT_PATH ../../lib)

add_library(AnomalyDetection ../src/detectanomalies.cpp ../include/detectanomalies.h)
add_executable(AnomalyDetectionCpp ${sourcefiles} ../src/main.cpp ../src/io.cpp ../src/detectanomalies.cpp ../src/statistics.cpp)