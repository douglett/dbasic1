SRC = main.cpp
HEAD = lexxer.hpp parsetools.hpp rule.hpp input.hpp

main.out: $(SRC) $(HEAD)
	g++ -std=c++14 -Wall -o main.out $(SRC)

run: main.out
	./main.out

clean:
	rm -f *.out *.o
	