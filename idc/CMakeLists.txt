# 自定义变量
set(PUBINCL ${PROJECT_SOURCE_DIR}/public)
set(PUBCPP ${PROJECT_SOURCE_DIR}/public/_public.cpp)
set(_MYSQLINCL ${PROJECT_SOURCE_DIR}/public/db)
set(_MYSQLCPP ${PROJECT_SOURCE_DIR}/public/db/_mysql.cpp)
set(MYSQLINCL /usr/local/mysql/include)
set(MYSQLLIB /usr/local/mysql/lib)

# 相当于g++ -I选项
include_directories(${PUBINCL} ${MYSQLINCL} ${_MYSQLINCL})
# 输出到指定目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# 编译选项
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_FLAGS "-lm -lc")
# -L添加库目录
link_directories(${MYSQLLIB})

add_executable(crtsurfdata cpp/crtsurfdata.cpp ${PUBCPP})
add_executable(obtcodetodb cpp/obtcodetodb.cpp ${PUBCPP} ${_MYSQLCPP})
add_executable(obtmindtodb cpp/obtmindtodb.cpp ${PUBCPP} ${_MYSQLCPP})

target_link_libraries(obtcodetodb mysqlclient)
target_link_libraries(obtmindtodb mysqlclient)
