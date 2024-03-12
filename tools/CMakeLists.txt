cmake_minimum_required(VERSION 3.14)
project(qxidc)

set(CMAKE_CXX_STANDARD 98)

# 自定义变量
set(PUBINCL ../public)
set(PUBCPP ../public/_public.cpp)
set(_MYSQLINCL ../public/db)
set(_MYSQLCPP ../public/db/_mysql.cpp)
set(MYSQLINCL /usr/local/mysql/include)
set(MYSQLLIB /usr/local/mysql/lib)

# 相当于g++ -I选项
include_directories(${PUBINCL} ${MYSQLINCL} ${_MYSQLINCL})
# -L添加库目录
link_directories(${MYSQLLIB})
# 输出到指定目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 编译选项
set(CMAKE_CXX_FLAGS "-lm -lc")

add_executable(procctl cpp/procctl.cpp)
add_executable(checkproc cpp/checkproc.cpp ${PUBCPP})
add_executable(gzipfiles cpp/gzipfiles.cpp ${PUBCPP})
add_executable(deletefiles cpp/deletefiles.cpp ${PUBCPP})
add_executable(execsql cpp/execsql.cpp ${PUBCPP} ${_MYSQLCPP})

target_link_libraries(execsql libmysqlclient.so)
