#pragma once
#include "parsetools.hpp"

struct Compiler : ParseTools {
	Node compile(const Node& prog) {
		Node n = {"()", { {"module $dbasic_program"} }};
		for (auto nn : prog.list[0].list)
			if (nn.val == "function") n.list.push_back(function(nn));
			else throw std::string("prog error");
		return n;
	}

	Node function(const Node& func) {
		auto name = func.find("name").find("IDENTIFIER").list[0].val;
		Node n = {"()", { 
			{"func"},
			{"$"+name},
			{"(export \""+name+"\")"},
			{"(result i32)"},
			{"(i32.const 0)"},
		}};
		return n;
	}

	// Node 
};