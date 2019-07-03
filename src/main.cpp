#include <iostream>
#include "parsing/tokenizer.hpp"
#include "parsing/parser.hpp"
#include "parsing/parseroutput.hpp"

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

//	test(); return 0;

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
	ParserOutput out;
	out.build(par.ast);
	std::cout << out.output.showlisp() << std::endl;
}