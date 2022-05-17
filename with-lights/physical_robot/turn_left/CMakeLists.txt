cmake_minimum_required(VERSION 3.16)
project(turn_left)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(turn_left
        main.cpp
        side.h)

find_package(YARP REQUIRED)

# we now add the YARP libraries to our project.
target_link_libraries(turn_left ${YARP_LIBRARIES})