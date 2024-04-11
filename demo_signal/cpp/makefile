all:book1 book2 book3

book1:book1.cpp
	g++ -g -o book1 book1.cpp 

book2:book2.cpp
	g++ -g -o book2 book2.cpp 

book3:book3.cpp
	g++ -g -o book3 book3.cpp /project/public/_public.cpp -I/project/public 

clean:
	rm -f book1 book2 book3
