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
			int res = func(ast.get("function", "main"));
			printf("main returns: %d\n", res);
			return res;
		}
		catch(const std::string& err) {
			return fprintf(stderr, "error: %s\n", err.c_str()), -1;
		}
	}

	StackFrame& global() { return sframe.at(0); }
	StackFrame& top()    { return sframe.at(max(1, int(sframe.size() - 1))); }
	void showframe(const StackFrame&) { for (auto& d : top()) printf("  %s : %d\n", d.first.c_str(), d.second); }

	int func(const ASTnode& fn) { // const warning
		sframe.push_back({});
		locals(fn.get("locals"));
		showframe(top());
		int res = block(fn.get("block"));
		showframe(top());
		sframe.pop_back();
		return res;
	}

	int locals(const ASTnode& locals) {
		for (auto d : locals.find("dim")) {
			if (top().count(d->value))
				throw std::string("duplicate definition: " + d->value);
			top()[d->value] = expr(d->get("expr"));
		}
		return 0;
	}

	int expr(const ASTnode& ex) {
		if (ex.type == "expr"      ) return expr(ex.children.at(0));
		if (ex.type == "number"    ) return std::stoi(ex.value);
		if (ex.type == "identifier") return getvar(ex.value);
		if (ex.type == "operator" && ex.value == "+") return expr(ex.children.at(0)) + expr(ex.children.at(1));
		if (ex.type == "operator" && ex.value == "-") return expr(ex.children.at(0)) - expr(ex.children.at(1));
		if (ex.type == "operator" && ex.value == "*") return expr(ex.children.at(0)) * expr(ex.children.at(1));
		if (ex.type == "operator" && ex.value == "/") return expr(ex.children.at(0)) / expr(ex.children.at(1));
		throw std::string("unknown in expr: " + ex.type + " " + ex.value);
	}

	int& getvar(const std::string& var) {
		if (top().count(var)) return top().at(var);
		// global
		throw std::string("missing identifier: " + var);
	}

	int block(const ASTnode& block) {
		for (auto& stmt : block.children) {
			if      (stmt.type == "assign") getvar(stmt.get("identifier").value) = expr(stmt.get("expr"));
			else if (stmt.type == "return") return expr(stmt.get("expr"));
			else    throw std::string("unexpected in block: " + stmt.type);
		}
		throw std::string("missing return value in block");
		//return 0; // default return value
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