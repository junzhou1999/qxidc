cmake_minimum_required(VERSION 3.14)
project(qxidc)

set(CMAKE_CXX_STANDARD 98)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/)
# 编译选项
set(CMAKE_CXX_FLAGS "-pthread")

add_executable(book01 book01.cpp)
add_executable(book02 book02.cpp)
add_executable(book03 book03.cpp)
add_executable(book04 book04.cpp)
add_executable(book05 book05.cpp)
add_executable(book06 book06.cpp)
add_executable(book07 book07.cpp)

