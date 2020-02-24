SRC = main.cpp
HEAD = ../parser/*.hpp *.hpp

main.out: ${SRC} ${HEAD}
	g++ -std=c++14 -Wall -o main.out ${SRC}

run: main.out
	./main.out
