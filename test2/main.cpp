#include <iostream>
#include "../parser/parser.hpp"
#include "runtime.hpp"
using namespace std;

int main() {
	printf("runtime test\n");

	Tokenizer t;
	t.load("test.bas");
	t.parse();
	//cout << t.show() << endl;

	Parser p(t);
	p.parse();
	cout << p.ast.show() << endl;

	Runtime r(p.ast);
	r.run();
}