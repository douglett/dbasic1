#pragma once
#include "../helpers/parsetools.hpp"
#include "../helpers/node.hpp"
#include "tokenizer.hpp"

struct ParserBase : ParseTools {
	Tokenizer tok;
	int pos = 0;

	// basic look-ahead
	const Token& peek(int offset=0) {
		return tok.tokens.at(pos + offset);
	}
	const std::string& peeks(int offset=0) {
		return peek(offset).val;
	}

	// parsing basic tokens
	int expect(const std::string& val) {
		if (peek().val == val)
			return ++pos, 1;
		return 0;
	}
	int expectm(const vecstr& vs) {
		for (auto& s : vs)
			if (!expect(s)) return 0;
		return 1;
	}
	int acceptm(const vecstr& vs) {
		int p = pos;
		if (expectm(vs)) return 1;
		return pos = p, 0;
	}
	int eoftok() {
		return peek().val == "[EOF]"; // does not consume EOF
	}
	int eoltok() {
		if (peek().val == "[EOL]")
			return ++pos, 1;
		return 0;
	}
	int linecomment() {
		// if (peek().val.substr(0, 2) == "//")
		if (peek().val[0] == '\'')
			return ++pos, 1;
		return 0;
	}
	int lineend() {
		linecomment();
		return eoltok() || eoftok();
	}
	int identifier() {
		if (isidentifier(peek().val))
			return ++pos, 1;
		return 0;
	}
	int number() {
		if (isnumber(peek().val))
			return ++pos, 1;
		return 0;
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