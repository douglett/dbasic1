#pragma once
#include "parsetools.hpp"
#include "rule.hpp"

struct Lexxer : ParseTools {
	std::vector<Rule> rules = {
		Rule::make("IDENTIFIER", "L(L|D)*"),
		Rule::make("NUMBER", "D+")
	};

	// int run(const Rule& rule, const std::string& str) {
	// 	int pos = 0;
	// 	return run(rule, str, pos);
	// }
	int run(const Rule& rule, const std::string& str, const int pos=0) {
		int len = 0, l = 0;
		for (const auto& r : rule.rule.list) {
			printf("running rule: %s\n", r.val.c_str());
			if      (r.val == "L"  && isletter(str[pos])) len++;
			else if (r.val == "D"  && isdigit(str[pos]) ) len++;
			// else if (r.val == "()" && (l = run(r, str, pos)) > -1) len+=l;
			return -1;
		}
		return 0;
	}
};