cmake_minimum_required(VERSION 3.16)
project(experiment)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(experiment
        main.cpp
        side.h
        serialport.h
        serialport.cpp)

find_package(YARP REQUIRED)

# we now add the YARP libraries to our project.
target_link_libraries(experiment ${YARP_LIBRARIES})
