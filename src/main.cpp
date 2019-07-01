#include <iostream>
#include "parsing/tokenizer.hpp"
#include "parsing/parser.hpp"

void test() {
	Node n = {"()", {
		{"module"}, {"$dbasic_program"},
		{"()", {
			{"function"}, {"$foo"}, {"()", { {"export \"foo\""} }}, {"()", {{"result i32"}} },
			{"()", {{"i32.const 0"}} }
		}}
	}};
	std::cout << n.showlisp() << std::endl;
}

int main() {
	printf("hello world\n");

	test(); return 0;

	Tokenizer tok;
	tok.load("test.bas") || tok.parse();
	printf("lines : %d\n", int(tok.lines.size()));
	printf("tokens: %d\n", int(tok.tokens.size()));

	Parser par(tok);
	par.parse();
	par.ast.showtmp(); //, printf("\n");
}