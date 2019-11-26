#include <iostream>
#include "parsing/tokenizer.hpp"
#include "parsing/parser.hpp"
#include "parsing/compiler.hpp"
#include "parsing/validate.hpp"

int main() {
	const std::string fname = "test.bas";
	// printf("hello world\n");
	printf("compiling %s ...\n", fname.c_str());

	printf("::tokenize::\n");
	Tokenizer tok;
	if (tok.load(fname)) return 1;
	if (tok.parse()) return 1;
	printf("  lines : %d\n", int(tok.lines.size()));
	printf("  tokens: %d\n", int(tok.tokens.size()));
	// std::cout << tok.show() << std::endl;

	printf("::parse::\n");
	Parser par(tok);
	if (par.parse() < 1) return 1;
	printf("  parse : ok\n");
	std::cout << par.ast.show() << std::endl;
	return 0;

	// printf("::validate::\n");
	// Validate val;
	// val.variables(par.ast);
	// return 0;

	// printf("::output::\n");
	// Compiler com;
	// if (com.build(par.ast)) return 1;
	// printf("  compile: ok\n");
	// // std::cout << com.output.showlisp() << std::endl;
	// std::fstream fs("test.wast", std::ios::out);
	// if (!fs.is_open()) return 1;
	// fs << com.output.showlisp();
	// printf("  output: ok\n");
}