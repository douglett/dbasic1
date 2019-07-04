#include "lexxer.hpp"
#include <iostream>

int main() {
	printf("hello world\n");

	Lexxer lex;
	lex.input.str("asd bacon cheese");
	lex.run("main");
}