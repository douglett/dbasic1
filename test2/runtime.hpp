#include <iostream>
#include <map>

struct Runtime {
	typedef  std::map<std::string, int32_t>  StackFrame;
	ASTnode ast;
	std::vector<StackFrame> sframe;

	Runtime(const ASTnode& n) { ast = n; }

	// load and run main function
	int run() {
		try {
			sframe = { {} }; // one empty global scope
			dim_globals();
			int res = func( ast.get("function", "main") );
			printf("main returns: %d\n", res);
			return res;
		}
		catch(const std::string& err) {
			return fprintf(stderr, "error: %s\n", err.c_str()), -1;
		}
	}

	// useful helper functions
	StackFrame& global() { return sframe.at(0); }
	StackFrame& top()    { return sframe.at(std::max(1, int(sframe.size() - 1))); }
	void showframe(const StackFrame& frame) { for (auto& d : frame) printf("  %s : %d\n", d.first.c_str(), d.second); }

	// get var from stack frame
	int& getvar(const std::string& var) {
		if (   top().count(var)) return    top().at(var); // local
		if (global().count(var)) return global().at(var); // global
		throw std::string("missing identifier: " + var);
	}

	int func(const ASTnode& fn) {
		sframe.push_back({ }); // push local stack frame
		dim_locals( fn.get("locals"), top() );
		showframe( top() ); // initial local state
		int res = block( fn.get("block") );
		showframe( top() ); // end local state
		sframe.pop_back(); // pop local frame
		return res;
	}

	int dim_globals() {
		int res = dim_locals( ast, global() );
		showframe( global() );
		return res;
	}

	int dim_locals(const ASTnode& locals, StackFrame& frame) {
		for (auto d : locals.find("dim")) {
			if (frame.count(d->value))
				throw std::string("duplicate definition: " + d->value);
			frame[d->value] = expr(d->get("expr"));
		}
		return 0;
	}

	int expr(const ASTnode& ex) {
		if (ex.type == "expr"      ) return expr(ex.children.at(0));
		if (ex.type == "number"    ) return std::stoi(ex.value);
		if (ex.type == "identifier") return getvar(ex.value);
		if (ex.type == "call"      ) return func(ast.get("function", ex.value));
		if (ex.type == "operator" && ex.value == "+") return expr(ex.children.at(0)) + expr(ex.children.at(1));
		if (ex.type == "operator" && ex.value == "-") return expr(ex.children.at(0)) - expr(ex.children.at(1));
		if (ex.type == "operator" && ex.value == "*") return expr(ex.children.at(0)) * expr(ex.children.at(1));
		if (ex.type == "operator" && ex.value == "/") return expr(ex.children.at(0)) / expr(ex.children.at(1));
		throw std::string("unknown in expr: " + ex.type + " " + ex.value);
	}

	int block(const ASTnode& block) {
		for (auto& stmt : block.children) {
			if      (stmt.type == "return") return expr(stmt.get("expr"));
			else if (stmt.type == "expr"  ) expr(stmt);
			// else if (stmt.type == "call"  ) expr(stmt);
			else if (stmt.type == "assign") getvar(stmt.get("identifier").value) = expr(stmt.get("expr"));
			else    throw std::string("unexpected in block: " + stmt.type);
		}
		throw std::string("missing return value in block");
		//return 0; // default return value
	}
};