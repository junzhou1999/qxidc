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
# 输出到指定目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 编译选项
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_FLAGS "-lm -lc")
# -L添加库目录
link_directories(${MYSQLLIB})

add_executable(crtsurfdata cpp/crtsurfdata.cpp ${PUBCPP})
add_executable(obtcodetodb cpp/obtcodetodb.cpp ${PUBCPP} ${_MYSQLCPP})

target_link_libraries(obtcodetodb mysqlclient)
