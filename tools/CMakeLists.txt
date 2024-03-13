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

set(_FTPINCL ../public/ftp)
set(_FTPCPP ../public/ftp/_ftp.cpp)
set(FTPLIB ../public/lib)

# 相当于g++ -I选项
include_directories(${PUBINCL} ${MYSQLINCL} ${_MYSQLINCL} ${_FTPINCL})
# -L添加库目录
link_directories(${MYSQLLIB} ${FTPLIB})
# 输出到指定目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 编译选项
set(CMAKE_CXX_FLAGS "-g -lm -lc")

add_executable(procctl cpp/procctl.cpp)
add_executable(checkproc cpp/checkproc.cpp ${PUBCPP})
add_executable(gzipfiles cpp/gzipfiles.cpp ${PUBCPP})
add_executable(deletefiles cpp/deletefiles.cpp ${PUBCPP})
add_executable(execsql cpp/execsql.cpp ${PUBCPP} ${_MYSQLCPP})
add_executable(ftpgetfiles cpp/ftpgetfiles.cpp ${PUBCPP} ${_FTPCPP})

target_link_libraries(execsql libmysqlclient.so)
target_link_libraries(ftpgetfiles libftp.a)
