#include <iostream>
#include <map>

struct Runtime {
	typedef  std::map<std::string, int32_t>  StackFrame;
	ASTnode ast;
	std::vector<StackFrame> sframe;
	std::vector<std::vector<int>> mem_heap = { {0} };

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
	StackFrame& global() { if (sframe.size() < 1) throw std::string("sackframe global() out-of-range");  return sframe.at(0); }
	StackFrame& top()    { if (sframe.size() < 2) throw std::string("sackframe top() out-of-range");     return sframe.at(sframe.size() - 1); }
	void showframe(const StackFrame& frame) { for (auto& d : frame) printf("  %s : %d\n", d.first.c_str(), d.second); }

	// get var from stack frame
	int& getvar(const std::string& var) {
		if (   top().count(var)) return    top().at(var); // local
		if (global().count(var)) return global().at(var); // global
		throw std::string("missing identifier: " + var);
	}


	// memory allocation and querying
	int mem_alloc(int size) {
		if (size < 1) throw std::string("mem_alloc: incorrect size: "+std::to_string(size));
		mem_heap.emplace_back(size, 0); // allocate to heap and zero memory
		return mem_heap.size() - 1;
	}
	int mem_free(int memloc) {
		if (memloc > 0 && memloc < (int)mem_heap.size())
			mem_heap[memloc] = {}; // reduce memloc size to zero
		//while (mem_heap.size() && mem_heap.back().size() == 0)
		//	mem_heap.pop_back(); // reduce heap size if possible
		return 0;
	}
	int mem_peek(int memloc, int offset) {
		if (memloc > 0 && offset >= 0 && memloc < (int)mem_heap.size() && offset < (int)mem_heap.size())
			return mem_heap.at(memloc).at(offset);
		throw std::string("mempeek: out of range: "+std::to_string(memloc)+", "+std::to_string(offset));
	}
	int mem_poke(int memloc, int offset, int value) {
		if (memloc > 0 && offset >= 0 && memloc < (int)mem_heap.size() && offset < (int)mem_heap.size())
			return mem_heap.at(memloc).at(offset) = value;
		throw std::string("mempoke: out of range: "+std::to_string(memloc)+", "+std::to_string(offset));
	}


	std::vector<int> getargs(const ASTnode& callsig, int count=-1) {
		std::vector<int> argv;
		for (auto ex : callsig.find("expr"))
			argv.push_back( expr(*ex) );
		if (count >= 0 && (int)argv.size() != count)
			throw std::string(callsig.value+": incorrect arg count: "+std::to_string(argv.size())+" (expected "+std::to_string(count)+")");
		return argv;
	}

	int sysfunc(const ASTnode& callsig) {
		if (callsig.value == "malloc") return mem_alloc(expr( callsig.get("expr") ));
//		if (callsig.value == "peek"  ) {
//			auto args = callsig.find("expr");
//			if (args.size() != 2) throw std::string("peek: incorrect number of arguments");
//			return mem_peek( expr(*args[0]), expr(*args[1]) );
//		}
//		if (callsig.value == "poke"  ) {
//			auto args = callsig.find("expr");
//			if (args.size() != 3) throw std::string("poke: incorrect number of arguments");
//			return mem_poke( expr(*args[0]), expr(*args[1]), expr(*args[2]) );
//		}

		if (callsig.value == "peek"  ) { auto argv = getargs(callsig, 2); return mem_peek(argv[0], argv[1]); }
		if (callsig.value == "poke"  ) { auto argv = getargs(callsig, 3); return mem_poke(argv[0], argv[1], argv[2]); }

		return func( ast.get("function", callsig.value) );
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
		//if (ex.type == "call"      ) return func(ast.get("function", ex.value));
		if (ex.type == "call"      ) return sysfunc(ex);
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