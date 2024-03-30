cmake_minimum_required(VERSION 3.14)
project(qxidc)

set(CMAKE_CXX_STANDARD 98)

add_subdirectory(public)
add_subdirectory(idc)
add_subdirectory(tools)
add_subdirectory(demo_signal)
add_subdirectory(demo_mysql)
add_subdirectory(demo_pthread)
