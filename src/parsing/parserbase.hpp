#pragma once
#include "../helpers/parsetools.hpp"
//#include "../helpers/node.hpp"
#include "tokenizer.hpp"

struct ParserBase : ParseTools {
	Tokenizer tok;
	int pos = 0;

	// basic look-ahead
	const Token& peek(int offset=0) {
		return tok.tokens.at(pos + offset);
	}
	int expect(const std::string& val) {
		if (peek().val == val)
			return ++pos, 1;
		return 0;
	}

	// parsing basic tokens
	int eoftok() {
		return peek().val == "[EOF]"; // does not consume EOF
	}
	int eoltok() {
		if (peek().val == "[EOL]")
			return ++pos, 1;
		return 0;
	}
	int identifier() {
		int start = 1;
		if (peek().val.length() == 0) return 0;
		for (char c : peek().val)
			if (start) { if (!isletter(c)) return 0; start = 0; }
			else if (!isletter(c) && !isdigit(c)) return 0;
		return ++pos, 1;
	}

	// error checking
	int doerr(const std::string& msg) {
		fprintf(stderr, "%s error line %d [%s]\n",
			msg.c_str(),
			peek().line,
			peek().val.c_str() );
		return -1;
	}
};