#include <iostream>
#include "parsing/tokenizer.hpp"
#include "parsing/parser.hpp"
#include "parsing/compiler.hpp"

int main() {
	printf("hello world\n");

	printf("::tokenize::\n");
	Tokenizer tok;
	tok.load("test.bas") || tok.parse();
	printf("lines : %d\n", int(tok.lines.size()));
	printf("tokens: %d\n", int(tok.tokens.size()));

	printf("::parse::\n");
	Parser par(tok);
	par.parse();
	std::cout << par.ast.showtmp() << std::endl;

	printf("::output::\n");
	Compiler com;
	com.build(par.ast);
	std::cout << com.output.showlisp() << std::endl;
}