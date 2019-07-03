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
			if   (fn.val == "function") p.list.push_back(function(fn));
			else throw std::string("expected function");
		return p;
	}

	Node function(const Node& fn) {
		auto name = fn.get("name").get(0).val;
		Node f = {"()", {
			{"function"},
			{"$"+name},
			{"(export \""+name+"\")"},
			{"(result i32)"}
		}};
//		fn.get("block").list.push_back
		f.list.push_back({"(i32.const 0)"});
		return f;
	}
};