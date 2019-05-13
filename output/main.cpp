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
		cout << "(\n" << string(indent, '\t');
		for (auto& nn : n.list)
			show_node(nn, indent+1);
		cout << ")";
	}
	else {
		cout << n.val << " ";
	}
}


// int write_block(Node& n) {

// }


int write_function(Node& n) {
	auto name = n.get("name").get(0).val;
	cout << name << endl;

	// printf("(func $%s (export %s) (result i32)\n", name.c_str(), name.c_str());
	// // write_block(Node& n);
	// printf("	(i32.const 0)\n");
	// printf(")\n");

	Node func = {"()", { 
		{"func"}, {"$"+name}, {"()", {
				{"export"}, {name}
			}},
			{"()", {
				{"block"}
			}},
			{"()", {
				{"result"}, {"i32"}
			}}
	}};
	show_node(func);

	return 0;
}


int main() {
	printf("hi\n");

	// function
	Node func = {"function", {
		{"name", { {"foo"} }},
		{"arguments"},
		{"block", { 
			// {"foo"} 
		}}
	}};


	write_function(func);
}