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
		Rule::make("IDENTIFIER", "L(L|D)*"),
		// language rules
		// // begin
		// Rule::make("main", "function"),
		// // functions
		// Rule::make("function", "WS~ 'function'~ name arguments EOL~ block function_end~"),
		// Rule::make("name", "WS~ IDENTIFIER WS~"),
		// Rule::make("arguments", "WS~ '('~ WS~ ')'~ WS~"),
		// Rule::make("function_end", "WS 'end' WS 'function' WS EOL"),
		// // block
		// Rule::make("block", "dim"),
		// Rule::make("dim", "WS~ 'dim'~ WS~ IDENTIFIER WS~ ('='~ WS~ NUMBER)? WS~ EOL~"),
	};
	Input input;

	/** helpers **/

	Rule& findrule(const std::string& rulename) {
		for (auto& r : rules)
			if (r.name == rulename) return r;
		auto err = "missing lexxer rule: "+rulename;
		std::cerr << err << std::endl;
		exit(1);
	}

	std::string stringify(const std::vector<Node>& list) {
		std::string s;
		for (auto& nn : list)
			s += nn.val + stringify(nn.list);
		return s;
	}

	int showerr(const std::string& msg) {
		fprintf(stderr, "[%s] error at position [%d]\n",
			msg.c_str(),
			(int)input.pos() );
		return 0;
	}

	/** parsing **/

	int run(const std::string& rulename, Node& n) {
		Rule& rule = findrule(rulename);
		// printf("running rule: %s\n", rulename.c_str());
		Node result = { rulename };
		if (runsub(rule.rule, result)) {
			if (rulename[0] >= 'A' && rulename[0] <= 'Z') // token - stringify it
				result.list = { {stringify(result.list)} };
			n.list.push_back(result);
			return 1;
		}
		return showerr(rulename);
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
		input.seek(pos);
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
		input.seek(pos);
		return runsub(rule.list[1], n);
	}

	int run_literal(const Node& rule, Node& n) {
		auto str = rule.val.substr(1, rule.val.length()-2);
		// printf("running string [%s]\n", str.c_str());
		int pos = input.pos();
		for (auto c : str)
			if   (input.peek() == c) input.get();
			else return input.seek(pos), 0;
		n.list.push_back({ str });
		return 1;
	}
};