cmake_minimum_required(VERSION 3.14)
project(qxidc)

set(CMAKE_CXX_STANDARD 98)

# 自定义变量
set(_MYSQLINCL ../public/db)
set(_MYSQLCPP ../public/db/_mysql.cpp)
set(MYSQLINCL /usr/local/mysql/include)
set(MYSQLLIB /usr/local/mysql/lib)

# 相当于g++ -I选项
include_directories(${_MYSQLINCL} ${MYSQLINCL})
# 输出到指定目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)
# 编译选项
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_FLAGS "-lm -lc")

# -L添加库目录
link_directories(${MYSQLLIB})

add_executable(createtable createtable.cpp ${_MYSQLCPP})
add_executable(inserttable inserttable.cpp ${_MYSQLCPP})
add_executable(updatetable updatetable.cpp ${_MYSQLCPP})
add_executable(selecttable selecttable.cpp ${_MYSQLCPP})
add_executable(deletetable deletetable.cpp ${_MYSQLCPP})
add_executable(filetoblob filetoblob.cpp ${_MYSQLCPP})
add_executable(dbblobtofile dbblobtofile.cpp ${_MYSQLCPP})

# 链接动态库
TARGET_LINK_LIBRARIES(createtable libmysqlclient.so)
TARGET_LINK_LIBRARIES(inserttable libmysqlclient.so)
TARGET_LINK_LIBRARIES(updatetable libmysqlclient.so)
TARGET_LINK_LIBRARIES(selecttable libmysqlclient.so)
TARGET_LINK_LIBRARIES(deletetable libmysqlclient.so)
TARGET_LINK_LIBRARIES(filetoblob libmysqlclient.so)
TARGET_LINK_LIBRARIES(dbblobtofile libmysqlclient.so)

# 先引用目录，在引用源代码和库文件
# g++ -g -o createtable createtable.cpp -I${_MYSQLINCL} -I${MYSQLINCL} $(_MYSQLCPP) -Llibmysqlclient.so
