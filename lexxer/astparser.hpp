#pragma once
#include "parsetools.hpp"
#include "rule.hpp"
#include "lexxer.hpp"

/**
 * AST Parser - takes input string and returns a parse tree
 */
struct ASTParser : ParseTools {
	std::vector<Rule> rules = {
		Rule::make("test1", "IDENTIFIER WS~ IDENTIFIER"),
		Rule::make("test2", "WS~ (IDENTIFIER WS~)+"),
	};
	Lexxer lex;

//	Rule& findrule(const std::string& rulename) {
//		for (auto& r : rules)
//			if (r.name == rulename) return r;
//		throw std::string("missing AST rule: "+rulename);
//	}
	Rule* findrulex(const std::string& rulename) {
		for (auto& r : rules)
			if (r.name == rulename) return &r;
		return NULL;
	}

	int runrule(const std::string& rulename, const std::string& str, const int pos, Node& result) {
		result = { rulename, { {} } };
		auto& res = result.list[0]; // actual result
		if (findrulex(rulename)) {
			printf("[%s] [%d]\n", rulename.c_str(), pos);
			return runsubrule(findrulex(rulename)->rule, str, pos, res);
		}
		else {
			printf("+[%s] [%d]\n", rulename.c_str(), pos);
			return runlex(rulename, str, pos, res);
		}
	}

	int runsubrule(const Node& rule, const std::string& str, const int pos, Node& result) {
		int len = 0, l = 0;
		Node tmp;
		result = { rule.val };
//		if      (pos + len >= (int)str.length()) return -1;
		if      (false) ;
		else if (rule.val == "~") return runsubrule(rule.list[0], str, pos, result);
		else if (rule.val == "+") {
			while ( (l = runsubrule(rule.list[0], str, pos + len, tmp)) > 0 )
				len += l, result.list.push_back(tmp);
			return len;
		}
		else if (rule.val == "()") {
			for (auto& r : rule.list) {
				l = runsubrule(r, str, pos + len, tmp);
				if (l == -1) return -1;
				if (r.val != "~") result.list.push_back(tmp);
				len += l;
			}
			return len;
		}
		return runrule(rule.val, str, pos, result);
	}

	int runlex(const std::string& rule, const std::string& str, const int pos, Node& result) {
		int len = lex.run(rule, str, pos);
		if (len > -1) result = { str.substr(pos, len) };
		return len;
	}
};