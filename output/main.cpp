#include <iostream>
#include <string>
#include <vector>
#include <node.hpp>
using namespace std;


namespace build {

Node b_if(const Node& ast) {
	return {};
}

Node b_block(const Node& ast) {
	Node block = {"()", { 
		{"block"}
	}};
	for (auto& ln : ast.list)
		if (ln.val == "if") ;
		else throw string("unexpected in block: "+ln.val);
	return block;
}

Node b_dim_block(const Node& ast) {
	Node dblock = {"()"};
	Node assign = {"()"};
	for (auto& ln : ast.list)
		; // ... TODO
	return dblock;
}

Node b_function(const Node& ast) {
	auto name = ast.get("name").get(0).val;
	auto block = b_block(ast.get("block"));
	// function template
	Node func = {"()", { 
		{"func"}, {"$"+name}, 
			{"()", { {"export"}, {name}	}}, // exported name
			{"()", { {"result"}, {"i32"} }}, // return value
	}};
	// define dim locals
	for (auto& d : ast.get("dim_block").list)
		func.list.push_back(
			{"()", {
				{"local"},
				{ "$"+d.get("name").get(0).val }
			}} );
	// set initial values
	for (auto& d : ast.get("dim_block").list)
		func.list.push_back(
			{"()", {
				{"local.set"}, 
				{ "$"+d.get("name").get(0).val },
				{"()", {
					{"i32.const"},
					{ d.get("expr").get(0).val }
				}}
			}} );
	// main block
	func.list.push_back(block);
	// append default return value
	func.list.push_back(
		{"()", {
			{"i32.const"}, {"0"}
		}} );
	return func;
}

Node b_module(const Node& ast) {
	Node module = {"()", {
		{"module"}, {"$dbasic_program"}
	}};
	// globals
	// (global $b (export "b") i32 (i32.const 0))
	// functions
	// (function $f)
	return module;
}

} // end build


int main() {
	printf("hi world\n");

	// function
	Node func = {"function", {
		{"name", { {"foo"} }},
		{"arguments"},
		{"dim_block", { 
			{"dim", {
				{"name", { {"a"} }},
				{"expr", { {"0"} }}
			}}
		}},
		{"block", { 
			{"if", {
				{"expr", {
					{"<", {
						{"a"}, {"10"}
					}}
				}},
				{"block", {
					{"set", {
						{"var", { {"a"} }},
						{"expr", {
							{"+", {
								{"var", { {"a"} }},
								{"number", { {"1"} }}
							}}
						}}
					}}
				}}
			}}
		}}
	}};

	try {
		Node prog = build::build_function(func);
		node_show(prog);
	}
	catch (const string& err) {
		cerr << "error: " << err << endl;
		return 1;
	}
}