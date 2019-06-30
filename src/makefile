SRC = main.cpp
HEAD = parsing/tokenizer.hpp parsing/parser.hpp helpers/parsetools.hpp helpers/node.hpp

main.out: $(SRC) $(HEAD)
	g++ -std=c++14 -Wall -o main.out $(SRC)

run: main.out
	./main.out

clean:
	rm -f *.out *.o
	