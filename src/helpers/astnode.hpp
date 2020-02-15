#pragma once
#include <string>
#include <vector>
#include <sstream>

struct ASTnode {
	std::string type, value;
	std::vector<ASTnode> children;

	ASTnode& push(const ASTnode& node) {
		children.push_back(node);
		return children.back();
	}

	ASTnode& get(const std::string& type) {
		for (auto& node : children)
			if (node.type == type)
				return node;
		// return NULL;
		throw std::string("not found: ["+type+"]");
	}

	std::vector<ASTnode*> find(const std::string& type) {
		std::vector<ASTnode*> result;
		for (auto& node : children)
			if (node.type == type) 
				result.push_back(&node);
		return result;
	}

	// helper
	std::string show(int indent=0) const {
		// auto s = std::string(indent*2, ' ') + "[" + type + "] " + value;
		// auto s = std::string(indent*2, ' ') + type + ": " + value;
		// auto s = std::string(indent*2, ' ') + type + (value.length() ? " ["+value+"]" : "");
		auto s = std::string(indent*2, ' ') + "\033[36m" + type + "\033[92m " + value + "\033[0m";
		for (auto& node : children)
			s += "\n" + node.show(indent + 1);
		return s;
	}
};