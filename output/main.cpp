#include <iostream>
#include <string>
#include <vector>
using namespace std;


struct Node {
	std::string val;
	std::vector<Node> list;

	int count() const {
		return list.size();
	}
	int count(const string& name) const {
		int c = 0;
		for (const auto& n : list)
			if (n.val == name) c++;
		return c;
	}
	// int get_index(const string& name, int index) const {
	// 	int c = 0;
	// 	for (int i = 0; i < list.size(); i++)
	// 		if (name == "*" || list[i].val == name) {
	// 			if (c == index) return i;
	// 			c++;
	// 		}
	// 	throw string("not found: ["+name+"] ["+to_string(index)+"] at ["+val+"]");
	// }
	// Node& get(int index) { return list[get_index("*", index)]; }
	// const Node& get(int index) const { return list[get_index("*", index)]; }
	// Node& get(const string& name, int index=0) { return list[get_index(name, index)]; }
	// const Node& get(const string& name, int index=0) const { return list[get_index(name, index)]; }
	Node& get(int index) {
		if (index < 0 || index >= list.size())
			throw string("not found: ["+to_string(index)+"] at ["+val+"]");
		return list[index];
	}
	Node& get(const string& name, int index=0) {
		int c = 0;
		for (auto& n : list)
			if (n.val == name) {
				if (c == index) return n;
				c++;
			}
		throw string("not found: ["+name+"] ["+to_string(index)+"] at ["+val+"]");
	}
};


void show_node(Node& n, int indent=0) {
	if (n.val == "()") {
		cout << (indent > 0 ? "\n" : "") << string(indent, '\t') << "(";
		for (auto& nn : n.list)
			show_node(nn, indent+1);
		cout << ")";
	}
	else {
		cout << n.val << " ";
	}
}


Node build_block(Node& ast) {
	Node block = {"()", { 
		{"block"}
	}};
	for (auto& ln : ast.list)
		if (ln.val == "if") ;
		else throw string("unexpected in block: "+ln.val);
	return block;
}

Node build_function(Node& ast) {
	auto name = ast.get("name").get(0).val;
	auto block = build_block(ast.get("block"));
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

Node build_module(Node& ast) {
	Node module = {"()", {
		{"module"}, {"$dbasic_program"}
	}};
	// globals
	// (global $b (export "b") i32 (i32.const 0))
	// functions
	// (function $f)
	return module;
}


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
		Node prog = build_function(func);
		show_node(prog);
	}
	catch (const string& err) {
		cerr << "error: " << err << endl;
		return 1;
	}
}