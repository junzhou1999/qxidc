# 编译器
CC = gcc
CXX = g++

# 编译选项
CFLAGS = -g
CXXFLAGS = -g -std=c++11

# 目标文件
TARGET = book01 book02 book03

# 编译运行
all:$(TARGET)

# 不使用自动推导，自动推导就是以下推导啥都不写，留空，make会自动根据选项来编译
# 使用 $@ 表示目标，$^ 表示所有依赖文件，$< 表示第一个依赖文件

book01:book01.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

book02:book02.cpp
	g++ -g -std=c++11 -o book02 book02.cpp 

book03:book03.cpp
	g++ -g -std=c++11 -o book03 book03.cpp 

# PHONY定义的目标代表不产生实际文件的目标，清理文件的目标也算是
.PHONY:clean

clean:
	rm -f $(TARGET)