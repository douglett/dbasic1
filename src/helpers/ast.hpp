#pragma once
#include <string>
#include <vector>
#include <sstream>

struct AST {
	std::string type, value;
	std::vector<AST> children;

	std::vector<AST*> find(const std::string& type) {
		std::vector<AST*> result;
		for (auto& node : children)
			if (node.type == type) 
				result.push_back(&node);
		return result;
	}


	// helper
	std::string show(int indent=0) const {
		auto s = std::string(indent*2, ' ') + "[" + type + "] " + value;
		for (auto& node : children)
			s += "\n" + node.show(indent + 1);
		return s;
	}
};