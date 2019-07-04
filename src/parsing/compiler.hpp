#pragma once
#include "../helpers/node.hpp"

struct Compiler {
	Node output;

	int build(const Node& ast) {
		output = prog(ast);
		return 0;
	}

	Node prog(const Node& ast) {
		Node p = {"()", {
			{"module"}, {"$dbasic_program"}
		}};
		for (auto& fn : ast.list)
			if   (fn.val == "function") p.push(function(fn));
			else throw std::string("expected function");
		return p;
	}

	Node function(const Node& func) {
		auto& name = func.get("name").get(0).val;
		Node fn = {"()", {
			{"func"},
			{"$"+name},
			{"(export \""+name+"\")"},
			{"(result i32)"}
		}};
		locals(func.get("locals"), fn); // local variables
		block(func.get("block"), fn); // function block
		fn.push({"()", { {"i32.const 0"} }}); // return value
		return fn;
	}

	void locals(const Node& locals, Node& fn) {
		Node local_def, local_set;
		for (auto& l : locals.list) {
			auto& name = l.get("name").get(0).val;
			local_def.pushs("()").pushs("local $"+name+" i32");
			auto& expr = l.get("expr").get(0).val;
			local_set.pushs("()").pushs("set_local $"+name+" (i32.const "+expr+")");
		}
		// add locals to function
		fn.list.insert(fn.list.end(), local_def.list.begin(), local_def.list.end());
		fn.list.insert(fn.list.end(), local_set.list.begin(), local_set.list.end());
	}

	void block(const Node& bl, Node& parent) {
		for (auto& b : bl.list)
			if   (false) ;
			else throw std::string("unexpected in block");
	}

//	Node dim(const Node& d) {
//		auto& name = d.get("name").get(0).val;
//		return {"()", { {"local $"+name+" i32"} }};
//	}
};