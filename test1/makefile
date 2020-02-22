SRC = main.cpp
PPATH = ../parser
# HEAD = \
# 	${PPATH}/tokenizer.hpp \
# 	${PPATH}/astnode.hpp ${PPATH}/parsetools.hpp \
# 	${PPATH}/parserbase.hpp ${PPATH}/parserexpression.hpp ${PPATH}/parser.hpp \
# 	${PPATH}/validate.hpp 
HEAD = ${PPATH}/*.hpp

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
	