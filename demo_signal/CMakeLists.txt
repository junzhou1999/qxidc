cmake_minimum_required(VERSION 3.14)
project(qxidc)

set(CMAKE_CXX_STANDARD 98)

# 自定义变量
set(PUBINCL ../public)
set(PUBCPP ../public/_public.cpp)

# 相当于g++ -I选项
 include_directories(${PUBINCL})
# 输出到指定目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 编译选项
# set(CMAKE_CXX_FLAGS "-lm -lc")

add_executable(book1 cpp/book1.cpp)
add_executable(book2 cpp/book2.cpp)
add_executable(bbook2 cpp/bbook2.cpp)
add_executable(book3 cpp/book3.cpp ${PUBCPP})
