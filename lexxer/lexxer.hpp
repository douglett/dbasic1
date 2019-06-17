#pragma once
#include "parsetools.hpp"
#include "rule.hpp"

struct Lexxer : ParseTools {
	std::vector<Rule> rules = {
		Rule::make("IDENTIFIER", "L(L|D)*"),
		Rule::make("NUMBER", "D+")
	};

	int run(const Rule& rule, const std::string& str) {
		std::string res;
		for (const auto& r : rule.rule.list) {
			std::cout << r.val << std::endl;
		}
		return 0;
	}
};
