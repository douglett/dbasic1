#include <iostream>
#include "../parser/parser.hpp"
using namespace std;

int main() {
	printf("hello world\n");
	Parser p;
	p.parsefile("test.bas");
	cout << p.tok.show() << endl;
	cout << p.ast.showc() << endl;
}