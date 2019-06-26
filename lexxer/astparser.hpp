#pragma once
#include "parsetools.hpp"
#include "rule.hpp"
#include "lexxer.hpp"

/**
 * AST Parser - takes input string and returns a parse tree
 */
struct ASTParser : ParseTools {
	std::vector<Rule> rules = {
		Rule::make("test1", "IDENTIFIER IDENTIFIER")
	};
	Lexxer lex;
	Node result;

	Rule& findrule(const std::string& rulename) {
		for (auto& r : rules)
			if (r.name == rulename) return r;
		throw std::string("missing rule: "+rulename);
	}
	
	void test() {
		auto& rule = findrule("test1");
		printf("here: %s:\n", rule.rule.val.c_str());
		std::string input = "asd blah";
		int pos = 0;
		Node n;

		for (auto& r : rule.rule.list) {
			pos += runlex("WS", input, pos); // eat whitespace between rules
			// printf("[%s]\n", r.val.c_str());
			int len = runlex(r.val, input, pos);
			printf("%d %s\n", len, result.val.c_str());
			pos += len;
		}
	}

	int runlex(const std::string& rule, const std::string& str, const int pos) {
		result = {};
		int len = lex.run(rule, str, pos);
		if (len > -1) result = { str.substr(pos, len) };
		return len;
	}
};