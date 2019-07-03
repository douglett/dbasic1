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

	Node function(const Node& fn) {
		auto& name = fn.get("name").get(0).val;
		Node f = {"()", {
			{"function"},
			{"$"+name},
			{"(export \""+name+"\")"},
			{"(result i32)"}
		}};
//		fn.get("block").push
		block(fn.get("block"), f);
		f.push({"()", { {"i32.const 0"} }}); // return value
		return f;
	}

	void block(const Node& bl, Node& parent) {
		for (auto& b : bl.list)
			if   (b.val == "dim") parent.push(dim(b));
			else throw std::string("unexpected in block");
	}

	Node dim(const Node& d) {
		auto& name = d.get("name").get(0).val;
		return {"()", { {"local $"+name+" i32"} }};
	}
};