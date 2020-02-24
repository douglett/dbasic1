#include <iostream>
#include <map>
#include "../parser/parser.hpp"
using namespace std;


struct Runtime {
	typedef  std::map<std::string, int32_t>  StackFrame;
	ASTnode ast;
	std::vector<StackFrame> sframe;

	Runtime(const ASTnode& n) { ast = n; }

	int run() {
		try {
			sframe = { {} }; // one empty global scope
			return func(ast.get("function", "main"));
		}
		catch(const std::string& err) {
			return fprintf(stderr, "error: %s\n", err.c_str()), -1;
		}
	}

	StackFrame& global() { return sframe.at(0); }
	StackFrame& top()    { return sframe.at(max(1, int(sframe.size() - 1))); }

	int func(ASTnode& fn) { // const warning
		sframe.push_back({});
		locals(fn.get("locals"));
		return 0;
	}

	int locals(ASTnode& locals) {
		for (auto d : locals.find("dim")) {
			printf("  dim %s\n", d->value.c_str());
			if (top().count(d->value))
				throw std::string("duplicate definition: " + d->value);
			top()[d->value] = 0;
		}
		for (auto p : top())
			printf("  %s : %d\n", p.first.c_str(), p.second);
		return 0;
	}
};


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