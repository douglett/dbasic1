#include "lexxer.hpp"
#include "compiler.hpp"
#include <iostream>
#include <fstream>

struct DBasic {
	Lexxer lex;
	Lexxer::Node ast;

	DBasic() {
		// create language rules
		std::vector<Rule> rl = {
			// entry
			Rule::make("main", "function"),
			Rule::make("emptyline", "WS~ EOL~"),
			// functions
			Rule::make("function", "WS~ 'function'~ name arguments EOL~ block function_end~"),
			Rule::make("name", "WS~ IDENTIFIER WS~"),
			Rule::make("arguments", "WS~ '('~ WS~ ')'~ WS~"),
			Rule::make("function_end", "WS 'end' WS 'function' WS EOL"),
			// block
			Rule::make("block", "(dim | emptyline~)*"),
			Rule::make("dim", "WS~ 'dim'~ WS~ IDENTIFIER WS~ ('='~ WS~ NUMBER)? WS~ EOL~"),
		};
		// insert into lexxer
		lex.rules.insert(lex.rules.end(), rl.begin(), rl.end());
	}

	int load() {
		return lex.input.load("test.bas");
	}

	int parse() {
		ast = { "result" };
		int res = lex.run("main", ast);
		// printf("result: %d\n", res);
		if (res) {
			lex.shownode(ast);
			Compiler com;
			auto output = com.compile(ast);
			// c.shownode(output);
			std::cout << com.showlisp(output) << std::endl;
			std::fstream fs("testout.wast", std::ios::out);
			fs << com.showlisp(output);
			return 0;
		}
		else {
			std::cerr << lex.error << std::endl
				<< lex.input.report() << std::endl;
			return 1; // invert - 1 = error
		}
	}
};


int main() {
	printf("hello world\n");

	// Lexxer lex;
	// lex.input.load("test.bas");
	// Lexxer::Node n = { "result" };
	// int i = lex.run("main", n);
	// printf("result: %d\n", i);
	// lex.shownode(n);

	DBasic db;
	if (db.load()) return 1;
	db.parse();
}