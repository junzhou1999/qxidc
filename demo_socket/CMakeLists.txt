cmake_minimum_required(VERSION 3.14)
project(qxidc)

set(CMAKE_CXX_STANDARD 98)


# 输出到指定目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/)
# 编译选项
set(CMAKE_CXX_FLAGS "-g")

add_executable(tcpselect tcpselect.cpp)
