# 自定义变量
set(PUBINCL ${PROJECT_SOURCE_DIR}/public)
set(PUBCPP ${PROJECT_SOURCE_DIR}/public/_public.cpp)

set(_MYSQLINCL ${PROJECT_SOURCE_DIR}/public/db)
set(_MYSQLCPP ${PROJECT_SOURCE_DIR}/public/db/_mysql.cpp)
set(MYSQLINCL /usr/local/mysql/include)
set(MYSQLLIB /usr/local/mysql/lib)

set(_FTPINCL ${PROJECT_SOURCE_DIR}/public/ftp)
set(_FTPCPP ${PROJECT_SOURCE_DIR}/public/ftp/_ftp.cpp)
set(FTPLIB ${PROJECT_SOURCE_DIR}/public/lib)

# 相当于g++ -I选项
include_directories(${PUBINCL} ${MYSQLINCL} ${_MYSQLINCL} ${_FTPINCL})
# -L添加库目录
link_directories(${MYSQLLIB} ${FTPLIB})
# 输出到指定目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
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
