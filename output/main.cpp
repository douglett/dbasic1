#include <iostream>
#include <string>
#include <vector>
#include <node.hpp>
using namespace std;


namespace build {

Node b_block(const Node& ast);

Node b_expr(const Node& ast) {
	Node e = {"()", {
		{"i32.const"},
		{ ast.get(0).val }  // TODO: better job here
	}};
	return e;
}

Node b_if(const Node& ast) {
	return {"()", {
		{"if"}, 
		b_expr ( ast.get("expr")  ),
		b_block( ast.get("block") )
	}};
}

Node b_block(const Node& ast) {
	Node block = {"()", { 
		{"block"}
	}};
	// parse each block command
	for (auto& ln : ast.list)
		if (ln.val == "if") block.list.push_back( b_if(ln) );
		else throw string("unexpected in block: "+ln.val);
	return block;
}

Node b_dim_block(const Node& ast) {
	Node dim = {"()"};
	// define dim locals
	for (auto& d : ast.list)
		dim.list.push_back(
			{"()", {
				{"local"},
				{ "$"+d.get("name").get(0).val }
			}} );
	// set initial values
	for (auto& d : ast.list)
		dim.list.push_back(
			{"()", {
				{"local.set"}, 
				{ "$"+d.get("name").get(0).val },
				b_expr( d.get("expr") )
			}} );
	return dim;
}

Node b_function(const Node& ast) {
	auto name = ast.get("name").get(0).val;
	// function template
	Node func = {"()", { 
		{"func"}, {"$"+name}, 
			{"()", { {"export"}, {name}	}}, // exported name
			{"()", { {"result"}, {"i32"} }}, // return value
	}};
	// define dim locals
	auto dim = b_dim_block(ast.get("dim_block"));
	func.list.insert(func.list.end(), dim.list.begin(), dim.list.end());
	// main block
	auto block = b_block(ast.get("block"));
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
	for (auto& f : ast.get("functions").list)
		module.list.push_back( b_function(f) );
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
					// {"set", {
					// 	{"var", { {"a"} }},
					// 	{"expr", {
					// 		{"+", {
					// 			{"var", { {"a"} }},
					// 			{"number", { {"1"} }}
					// 		}}
					// 	}}
					// }}
				}}
			}}
		}}
	}};

	try {
		Node prog = build::b_function(func);
		node_show(prog);
	}
	catch (const string& err) {
		cerr << "error: " << err << endl;
		return 1;
	}
}