#pragma once
#include "../helpers/node.hpp"

struct Validate {
	int variables(const Node& ast) {
		std::cout << ast.showtmp() << std::endl;

		for (auto& n : ast.list)
			if (n.val == "function")
				var_func(n);

		return 0;
	}

	int var_func(const Node& ast) {
		// locals list
		std::vector<std::string> locals;
		// find locals
		auto& nlocals = ast.get("locals");
		for (auto& dim : nlocals.list)
			std::cout << dim.get("name").get(0).val << std::endl;

		return 0;
	}
};