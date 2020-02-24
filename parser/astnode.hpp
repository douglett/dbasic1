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

	ASTnode pop() {
		if (children.size() == 0) throw std::string("pop from empty ASTnode");
		ASTnode n = children.back();
		children.pop_back();
		return n;
	}

	ASTnode& get(const std::string& type, const std::string& value="") {
		for (auto& node : children)
			if (node.type == type && (value == "" || node.value == value))
				return node;
		throw std::string("not found: ["+type+"]"+(value == "" ? "" : " ["+value+"]"));
	}
	const ASTnode& get(const std::string& type, const std::string& value="") const {
		for (auto& node : children)
			if (node.type == type && (value == "" || node.value == value))
				return node;
		throw std::string("not found: ["+type+"]");
	}

	std::vector<ASTnode*> find(const std::string& type) {
		std::vector<ASTnode*> result;
		for (auto& node : children)
			if (node.type == type)
				result.push_back(&node);
		return result;
	}
	std::vector<const ASTnode*> find(const std::string& type) const {
		std::vector<const ASTnode*> result;
		for (auto& node : children)
			if (node.type == type)
				result.push_back(&node);
		return result;
	}

	// helper
	std::string show(int indent=0) const {
		auto s = std::string(indent*2, ' ') + type + (value.length() ? " ["+value+"]" : "");
		for (auto& node : children)
			s += "\n" + node.show(indent + 1);
		return s;
	}
	std::string showc(int indent=0) const {
		auto s = std::string(indent*2, ' ') + "\033[36m" + type + "\033[92m " + value + "\033[0m";
		for (auto& node : children)
			s += "\n" + node.showc(indent + 1);
		return s;
	}
};