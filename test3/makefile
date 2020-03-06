SRC = main.cpp
HEAD = ../parser/*.hpp
OUT = main.out

$(OUT): $(SRC) $(HEAD)
	g++ -std=c++14 -Wall -o $(OUT) $(SRC)

run: $(OUT)
	./$(OUT)