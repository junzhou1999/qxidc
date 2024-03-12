cmake_minimum_required(VERSION 3.14)
project(qxidc)

set(CMAKE_CXX_STANDARD 98)

# 设置输出库的文件目录
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(ftp STATIC ftp/ftplib.c)

# 输出信息
message(${CMAKE_CURRENT_SOURCE_DIR}/lib)

