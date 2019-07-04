#pragma once
#include "parsetools.hpp"
#include "rule.hpp"
#include <sstream>

/**
 * Lexxer - takes input string and returns a token string
 */
struct Lexxer : ParseTools {
	std::vector<Rule> rules = {
		Rule::make("WS", "S*"),
		Rule::make("NUMBER", "D+"),
		Rule::make("IDENTIFIER", "L(L|D)*"),
		Rule::make("main", "(WS IDENTIFIER WS)+")
	};
	std::stringstream input;

	Rule& findrule(const std::string& rulename) {
		for (auto& r : rules)
			if (r.name == rulename) return r;
		auto err = "missing lexxer rule: "+rulename;
		std::cerr << err << std::endl;
		exit(1);
	}

	int peek() {
		int i = input.tellg();
		int j = input.peek();
		input.seekg(i);
		return j;
	}

	int showerr(const std::string& msg) {
		fprintf(stderr, "[%s] error at position [%d]\n",
			msg.c_str(),
			(int)input.tellg() );
		return 0;
	}

	int run(const std::string& rulename, Node& n) {
		Rule& rule = findrule(rulename);
		n.list.push_back({ rulename });
		printf("running rule: %s\n", rulename.c_str());
		if (runsub(rule.rule, n.list.back())) {
			//printf("  [%d] [%d] [%s]\n", pos, (int)input.tellg(), "?");
			if (rulename[0] >= 'A' && rulename[0] <= 'Z')
				n.list.back() = { stringify(n.list.back()) }; // token - stringify it
			return 1;
		}
		return showerr(rulename);
	}

	int runsub(const Node& rule, Node& n) {
		// printf("[%s]\n", rule.val.c_str());
		if (inlist(rule.val, {"S", "L", "D"})) return run_char(rule, n);
//		if (rule.val == "S") return run_char(rule, "S", n);
//		if (rule.val == "L") return run_char(rule, "L", n);
//		if (rule.val == "D") return run_char(rule, "D", n);
		if (rule.val == "()") return run_brackets(rule, n);
		if (rule.val == "+") return run_plus(rule, n);
		if (rule.val == "*") return run_mul(rule, n);
		if (rule.val == "|") return run_or(rule, n);
		return run(rule.val, n); // run rule by name
	}

	int run_char(const Node& rule, Node& n) {
		int res = 0;
		if (rule.val == "S") res = isspace(peek());
		if (rule.val == "L") res = isletter(peek());
		if (rule.val == "D") res = isdigit(peek());
		if (res) {
			n.list.push_back({ std::string()+(char)input.get() });
			return 1;
		}
		return 0;
	}

	int run_brackets(const Node& rule, Node& n) {
		for (auto r : rule.list)
			if (!runsub(r, n)) return 0;
		return 1;
	}

	int run_plus(const Node& rule, Node& n) {
		if (!runsub(rule.list[0], n)) return 0;
		return run_mul(rule, n);
	}

	int run_mul(const Node& rule, Node& n) {
		int pos = input.tellg();
		while (runsub(rule.list[0], n))
			pos = input.tellg();
		input.seekg(pos);
		return 1;
	}

	int run_or(const Node& rule, Node& n) {
		int pos = input.tellg();
		if (runsub(rule.list[0], n)) return 1;
		input.seekg(pos);
		return runsub(rule.list[1], n);
	}

	std::string stringify(Node& n) {
		std::string s = n.val;
		for (auto& nn : n.list)
			s += stringify(nn);
		return s;
	}
};