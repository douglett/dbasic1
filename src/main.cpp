#include <iostream>
#include "parsing/tokenizer.hpp"
#include "parsing/parser.hpp"

int main() {
	printf("hello world\n");

	Tokenizer tok;
	tok.load("test.bas") || tok.parse();
	printf("lines : %d\n", int(tok.lines.size()));
	printf("tokens: %d\n", int(tok.tokens.size()));

	Parser par(tok);
	par.parse();
	par.ast.showtmp(); //, printf("\n");
}