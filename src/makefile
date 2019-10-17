SRC = main.cpp
HEAD = helpers/parsetools.hpp helpers/node.hpp \
	parsing/tokenizer.hpp \
	parsing/parserbase.hpp parsing/parserexpression.hpp parsing/parser.hpp \
	parsing/validate.hpp \
	parsing/compiler.hpp

main.out: $(SRC) $(HEAD)
	g++ -std=c++14 -Wall -o main.out $(SRC)

run: main.out
	./main.out

asm: test.wast
	# wasm-as test.wast
	wa assemble -o test.wasm test.wast

node: test.wast
	node test-wasm.js

clean:
	rm -f *.out *.o
	