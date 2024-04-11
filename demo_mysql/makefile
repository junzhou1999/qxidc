all:createtable updatetable inserttable deletetable selecttable dbblobtofile filetoblob

# mysql头文件存放路径
MYINCL = -I/usr/local/mysql/include

# mysql库文件保存路径
MYLIB = -L/usr/local/mysql/lib

# mysql指定使用的库，默认动态库
MYLIBS = -lmysqlclient

# 框架文件路径
_MYPATH = /project/public/db

# 封装好的框架的mysql头文件存放路径
_MYINCL = -I$(_MYPATH)

# 封装好的mysql库文件保存路径
_MYCPP = $(_MYPATH)/_mysql.cpp

CFLAGS = -g -Wno-write-strings

createtable:createtable.cpp
	g++ $(CFLAGS) -o createtable createtable.cpp $(_MYCPP) $(MYINCL) $(_MYINCL) $(MYLIB) $(MYLIBS)

updatetable:updatetable.cpp
	g++ $(CFLAGS) -o updatetable updatetable.cpp $(_MYCPP) $(MYINCL) $(_MYINCL) $(MYLIB) $(MYLIBS)

inserttable:inserttable.cpp
	g++ $(CFLAGS) -o inserttable inserttable.cpp $(_MYCPP) $(MYINCL) $(_MYINCL) $(MYLIB) $(MYLIBS)

deletetable:deletetable.cpp
	g++ $(CFLAGS) -o deletetable deletetable.cpp $(_MYCPP) $(MYINCL) $(_MYINCL) $(MYLIB) $(MYLIBS)

selecttable:selecttable.cpp
	g++ $(CFLAGS) -o selecttable selecttable.cpp $(_MYCPP) $(MYINCL) $(_MYINCL) $(MYLIB) $(MYLIBS)

dbblobtofile:dbblobtofile.cpp
	g++ $(CFLAGS) -o dbblobtofile dbblobtofile.cpp $(_MYCPP) $(MYINCL) $(_MYINCL) $(MYLIB) $(MYLIBS)

filetoblob:filetoblob.cpp
	g++ $(CFLAGS) -o filetoblob filetoblob.cpp $(_MYCPP) $(MYINCL) $(_MYINCL) $(MYLIB) $(MYLIBS)

clean:
	rm -f createtable updatetable inserttable deletetable selecttable dbblobtofile filetoblob