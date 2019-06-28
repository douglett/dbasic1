#pragma once
#include <string>
#include <vector>

struct Node {
	std::string val;
	std::vector<Node> list;


	// helpers
	int count() const {
		return list.size();
	}
	int count(const std::string& name) const {
		int c = 0;
		for (const auto& n : list)
			if (n.val == name) c++;
		return c;
	}
	int get_index(const std::string& name, int index) const {
		int c = 0;
		for (int i = 0; i < list.size(); i++)
			if (name == "*" || list[i].val == name) {
				if (c == index) return i;
				c++;
			}
		throw std::string("not found: ["+name+"] ["+std::to_string(index)+"] at ["+val+"]");
	}

	// accessors
	Node& get(int index) { 
		return list[ get_index("*", index) ]; 
	}
	Node& get(const std::string& name, int index=0) { 
		return list[ get_index(name, index) ]; 
	}

	// const accessors
	const Node& get(int index) const { 
		return list[ get_index("*", index) ]; 
	}
	const Node& get(const std::string& name, int index=0) const { 
		return list[ get_index(name, index) ]; 
	}
};


// node helpers
void node_show(Node& n, int indent=0) {
	if (n.val == "()") {
		std::cout << (indent > 0 ? "\n" : "") << std::string(indent, '\t') << "(";
		for (auto& nn : n.list)
			node_show(nn, indent+1);
		std::cout << ")";
	}
	else {
		std::cout << n.val << " ";
	}
}