SRC = main.cpp
HEAD = helpers/parsetools.hpp helpers/node.hpp \
	parsing/tokenizer.hpp parsing/parserbase.hpp parsing/parser.hpp \
	parsing/compiler.hpp

main.out: $(SRC) $(HEAD)
	g++ -std=c++14 -Wall -o main.out $(SRC)

run: main.out
	./main.out

clean:
	rm -f *.out *.o
	