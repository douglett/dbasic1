#include <iostream>
#include "parsing/tokenizer.hpp"
#include "parsing/parser.hpp"
#include "parsing/compiler.hpp"

int main() {
	printf("hello world\n");

	printf("::tokenize::\n");
	Tokenizer tok;
	if (tok.load("test.bas")) return 1;
	if (tok.parse()) return 1;
	printf("  lines : %d\n", int(tok.lines.size()));
	printf("  tokens: %d\n", int(tok.tokens.size()));
	// std::cout << tok.show() << std::endl;

	printf("::parse::\n");
	Parser par(tok);
	if (par.parse() < 1) return 1;
	printf("  parse : ok\n");
	// std::cout << par.ast.showtmp() << std::endl;
	// return 0;

	printf("::output::\n");
	Compiler com;
	if (com.build(par.ast)) return 1;
	std::cout << com.output.showlisp() << std::endl;
	std::fstream fs("test.wast", std::ios::out);
	fs << com.output.showlisp();
}