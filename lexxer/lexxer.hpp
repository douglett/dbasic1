#pragma once
#include "parsetools.hpp"
#include "rule.hpp"

/**
 * Lexxer - takes input string and returns a token string
 */
struct Lexxer : ParseTools {
	std::vector<Rule> rules = {
		Rule::make("WS", "S*"),
		Rule::make("NUMBER", "D+"),
		Rule::make("IDENTIFIER", "L(L|D)*")
	};

	Rule& findrule(const std::string& rulename) {
		for (auto& r : rules)
			if (r.name == rulename) return r;
		throw std::string("missing lexxer rule: "+rulename);
	}

	int run(const std::string& rulename, const std::string& str, const int pos=0) {
		Rule& rule = findrule(rulename);
		return runsub(rule.rule, str, pos);
	}

	int runsub(const Node& rule, const std::string& str, const int pos=0) {
		int len = 0, l = 0;
		// printf("running subrule: %s  [%s] [%d]\n", rule.val.c_str(), str.c_str(), pos);
//		if      (pos >= (int)str.length()) return -1;
		if      (false) ;
		else if (rule.val == "S") return !iseof(str, pos) && isspace(str[pos])  ? 1 : -1;
		else if (rule.val == "L") return !iseof(str, pos) && isletter(str[pos]) ? 1 : -1;
		else if (rule.val == "D") return !iseof(str, pos) && isdigit(str[pos])  ? 1 : -1;
		else if (rule.val == "|") {
			if ( (l = runsub(rule.list[0], str, pos)) > -1 ) return l;
			if ( (l = runsub(rule.list[1], str, pos)) > -1 ) return l;
			return -1;
		}
		else if (rule.val == "*") {
			while ( (l = runsub(rule.list[0], str, pos + len)) > 0 ) len += l;
			return len;
		}
		else if (rule.val == "+") {
			while ( (l = runsub(rule.list[0], str, pos + len)) > 0 ) len += l;
			return len >= 1 ? len : -1;
		}
		else if (rule.val == "()") {
			for (const auto& r : rule.list) {
				l = runsub(r, str, pos + len);
				if (l == -1) return -1;
				len += l;
			}
			return len;
		}
		return run(rule.val, str, pos);
	}
};