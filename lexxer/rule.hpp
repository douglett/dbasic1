#pragma once
#include "parsetools.hpp"

struct Rule : ParseTools {
	Node rule;
	vecstr tokens;
	unsigned int pos = 0;
	const Node nEOF = { "[EOF]" };

	static Rule make(const std::string& name, const std::string& rulestr) {
		Rule r;
		r.mkrule(name, rulestr);
		return r;
	}

	int mkrule(const std::string& name, const std::string& rulestr) {
		try {
			rule = { name };
			tokens = split(rulestr);
			pos = 0;
			while (pos < tokens.size())
				rule.list.push_back(mkor());
			shownode(rule); // debugging
			return 0;
		} catch (const std::string& err) {
			fprintf(stderr, "rule error: %s\n", err.c_str());
			return 1;
		}
	}
	Node mkor() {
		auto n = mkmodifier();
		if (pos < tokens.size() && tokens[pos] == "|") {
			pos++;
			auto n2 = mkatom();
			n = { "|", { n, n2 } };
		}
		return n;
	}
	Node mkmodifier() {
		Node n = mkatom();
		if (pos < tokens.size() && is_modifier(tokens[pos]))
			n = { tokens[pos++], { n } };
		return n;
	}
	int is_modifier(const std::string& s) {
		return s == "*" || s == "+" || s == "?";
	}
	Node mkatom() {
		if (pos >= tokens.size())
			throw std::string("unexpected end-of-line");
		if (tokens[pos] == "(")
			return mkbrackets();
		Node n = { tokens[pos++] };
		// checking
		return n;
	}
	Node mkbrackets() {
		Node n = { "()" };
		pos++;
		while (pos < tokens.size())
			if   (tokens[pos] == ")") return ++pos, n;
			else n.list.push_back({ tokens[pos++] });
		throw std::string("expected end bracket");
	}
};