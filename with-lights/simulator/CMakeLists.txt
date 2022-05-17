cmake_minimum_required(VERSION 3.16)
project(simulator)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(simulator
        main.cpp
        side.h serialport.cpp serialport.h)

find_package(YARP REQUIRED)

# we now add the YARP libraries to our project.
target_link_libraries(simulator ${YARP_LIBRARIES})
