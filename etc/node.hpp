#pragma once
#include <string>
#include <vector>
#include <sstream>

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
		for (unsigned int i = 0; i < list.size(); i++)
			if (name == "*" || list[i].val == name) {
				if (c == index) return i;
				c++;
			}
		throw std::string("not found: ["+name+"] ["+std::to_string(index)+"] at ["+val+"]");
	}

	// mutators
	Node& push(const Node& n) {
		list.push_back(n);
		return list.back();
	}
	Node& pushs(const std::string& s) {
		list.push_back({ s });
		return list.back();
	}
	Node& append(const std::vector<Node>& list2) {
		list.insert(list.end(), list2.begin(), list2.end());
		return *this;
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

	// node helpers - show as lisp
	std::string showlisp(int indent=0) const {
		std::string s;
		if (val == "()") {
			s = (indent > 0 ? "\n" : "") + std::string(indent, '\t') + "(";
			for (auto& nn : list)
				s += nn.showlisp(indent+1);
			s += ")";
		}
		else {
			s += val + " ";
		}
		return s;
	}
	std::string showtmp(int indent=0) const {
		auto s = std::string(indent*2, ' ') + val;
		for (auto& n : list)
			s += "\n" + n.showtmp(indent + 1);
		return s;
	}
};