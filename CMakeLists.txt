cmake_minimum_required(VERSION 3.12)
project(riscV_SIM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -O3 -gdwarf-3")     #-gdwarf-3

add_executable( riscV_SIM
        skeleton.cpp)

