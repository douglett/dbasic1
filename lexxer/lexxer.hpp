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

	int showerr(const std::string& msg) {
		fprintf(stderr, "[%s] error at position [%d]\n",
			msg.c_str(),
			(int)input.tellg() );
		return 0;
	}

	int run(const std::string& rulename) {
		Rule& rule = findrule(rulename);
		int pos = input.tellg();
		printf("running rule: %s\n", rulename.c_str());
		if (runsub(rule.rule)) {
//			printf("  [%s]\n", 
			return 1;
		}
		return showerr(rulename);
	}

	int runsub(const Node& rule) {
		// printf("[%s]\n", rule.val.c_str());
		if (rule.val == "S") return isspace(input.peek() ) ? (input.get(), 1) : 0;
		if (rule.val == "L") return isletter(input.peek()) ? (input.get(), 1) : 0;
		if (rule.val == "D") return isdigit(input.peek() ) ? (input.get(), 1) : 0;
		if (rule.val == "()") return run_brackets(rule);
		if (rule.val == "+") return run_plus(rule);
		if (rule.val == "*") return run_mul(rule);
		if (rule.val == "|") return run_or(rule);
		return run(rule.val); // run rule by name
	}

	int run_brackets(const Node& rule) {
		for (auto r : rule.list)
			if (!runsub(r)) return 0;
		return 1;
	}

	int run_plus(const Node& rule) {
		if (!runsub(rule.list[0])) return 0;
		return run_mul(rule);
	}

	int run_mul(const Node& rule) {
		int pos = input.tellg();
		while (runsub(rule.list[0]))
			pos = input.tellg();
		input.seekg(pos);
		return 1;
	}

	int run_or(const Node& rule) {
		int pos = input.tellg();
		if (runsub(rule.list[0])) return 1;
		input.seekg(pos);
		return runsub(rule.list[1]);
	}
};