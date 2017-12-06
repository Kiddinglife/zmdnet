
cmake_minimum_required(VERSION 3.0)
project(zmdnetlib C) # C PROJECT

# debug is default build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, using DEBUG")
    set(CMAKE_BUILD_TYPE "DEBUG")
endif()