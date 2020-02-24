#include <iostream>
#include "../parser/parser.hpp"
#include "runtime.hpp"
using namespace std;

int main() {
	printf("runtime test\n");

	Tokenizer t;
	if (t.load("test.bas") || t.parse()) return 1;
	//cout << t.show() << endl;

	Parser p(t);
	if (p.parse()) return 1;
	cout << p.ast.showc() << endl;

	Runtime r(p.ast);
	r.run();
}