#pragma once
#include "parsetools.hpp"
#include "rule.hpp"
#include "input.hpp"

/**
 * Lexxer - takes input string and returns a token string
 */
struct Lexxer : ParseTools {
	std::vector<Rule> rules = {
		// default rules
		Rule::make("WS", "S*"),
		Rule::make("NUMBER", "D+"),
		Rule::make("IDENTIFIER", "L(L|D)*")
	};
	Input input;
	std::string error;

	/** helpers **/

	Rule& findrule(const std::string& rulename) {
		for (auto& r : rules)
			if (r.name == rulename) return r;
		auto err = "missing lexxer rule: "+rulename;
		std::cerr << err << std::endl;
		exit(1);
	}

	int showerr(const std::string& msg) {
		fprintf(stderr, "error rule [%s], line [%d]\n",
			msg.c_str(),
			input.pos().line+1 );
		return 0;
	}

	std::string stringify(const std::vector<Node>& list) {
		std::string s;
		for (auto& nn : list)
			s += nn.val + stringify(nn.list);
		return s;
	}

	std::string stringify2(const Node& n) {
		if (n.list.size() == 0) return n.val;
		std::string s;
		for (auto nn : n.list)
			s += stringify2(nn);
		return s;
	}

	/** parsing **/

	int run(const std::string& rulename, Node& n) {
		Rule& rule = findrule(rulename);
		// printf("running rule: %s\n", rulename.c_str());
		Node result = { rulename };
		if (runsub(rule.rule, result)) {
			if (rulename[0] >= 'A' && rulename[0] <= 'Z') // token - stringify it
				result.list = { {stringify2(result)} };
			n.list.push_back(result);
			return 1;
		}
		error = "["+rulename+"] > " + error;
		// return showerr(rulename);
		return 0;
	}

	int runsub(const Node& rule, Node& n) {
		// printf("[%s]\n", rule.val.c_str());
		if (inlist(rule.val, {"S", "L", "D", "EOL", "EOF"})) return run_char(rule, n);
		if (rule.val == "()") return run_brackets(rule, n);
		if (rule.val == "~") return run_exclude(rule);
		if (rule.val == "?") return run_optional(rule, n);
		if (rule.val == "*") return run_mul(rule, n);
		if (rule.val == "+") return run_plus(rule, n);
		if (rule.val == "|") return run_or(rule, n);
		if (rule.val[0] == '\'' && rule.val.back() == '\'') return run_literal(rule, n);
		return run(rule.val, n); // run rule by name
	}

	int run_char(const Node& rule, Node& n) {
		int res = 0;
		if (rule.val == "S") res = iswspace(input.peek());
		if (rule.val == "L") res = isletter(input.peek());
		if (rule.val == "D") res = isdigit(input.peek());
		if (rule.val == "EOL") res = iseol(input.peek());
		if (rule.val == "EOF") res = iseof(input.peek());
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

	int run_exclude(const Node& rule) {
		Node n;
		return runsub(rule.list[0], n);
	}

	int run_optional(const Node& rule, Node& n) {
		auto pos = input.pos();
		if (runsub(rule.list[0], n)) return 1;
		input.seek(pos), error = "";
		return 0;
	}

	int run_mul(const Node& rule, Node& n) {
		while (run_optional(rule, n)) ;
		return 1;
	}

	int run_plus(const Node& rule, Node& n) {
		if (!runsub(rule.list[0], n)) return 0;
		return run_mul(rule, n);
	}

	int run_or(const Node& rule, Node& n) {
		auto pos = input.pos();
		if (runsub(rule.list[0], n)) return 1;
		input.seek(pos), error = "";
		return runsub(rule.list[1], n);
	}

	int run_literal(const Node& rule, Node& n) {
		auto str = rule.val.substr(1, rule.val.length()-2);
		// printf("running string [%s]\n", str.c_str());
		auto pos = input.pos();
		for (auto c : str)
			if   (input.peek() == c) input.get();
			else return input.seek(pos), 0;
		n.list.push_back({ str });
		return 1;
	}
};