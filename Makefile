comp: lex.yy.c binary.tab.o main.cc
	g++ -std=gnu++11 -g -ocomp binary.tab.o lex.yy.c main.cc node.cpp convert.cpp
binary.tab.o: binary.tab.cc
	g++ -std=gnu++11 -g -c binary.tab.cc
binary.tab.cc: binary.yy
	bison binary.yy -v
lex.yy.c: binary.ll binary.tab.cc
	flex binary.ll
clean: 
	rm -f binary.tab.* lex.yy.c* comp