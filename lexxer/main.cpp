#include "lexxer.hpp"
#include <iostream>

int main() {
	printf("hello world\n");

	Lexxer lex;
	lex.input.str("asd bacon cheese");
	Lexxer::Node n;
	int i = lex.run("main", n);
	printf("result: %d\n", i);
	lex.shownode(n);
}