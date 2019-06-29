#pragma once
#include "../helpers/parsetools.hpp"
#include "tokenizer.hpp"

struct Parser : ParseTools {
	Tokenizer tok;
	int pos = 0;

	Parser(const Tokenizer& t) { tok = t; }

	int parse() {
		for (auto t : tok.tokens)
			printf("  [%s]\n", t.val.c_str());
		int result = prog();
		printf("parse result: %d\n", result);
		return 0;
	}

	const Token& peek(int offset=0) {
		return tok.tokens.at(pos + offset);
	}

	int doerr(const std::string& msg) {
		fprintf(stderr, "%s error line %d [%s]\n",
			msg.c_str(),
			peek().line,
			peek().val.c_str() );
		return -1;
	}

	int prog() {
		while (!eoftok())
			if      (eoltok() > 0) ;
			else if (function() > 0) ;
			else    goto err;
		return 1;
		err:
		fprintf(stderr, "root-scope error line %d\n", peek().line);
		return -1;
	}

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

	int expect(const std::string& val) {
		if (peek().val == val)
			return ++pos, 1;
		return 0;
	}

	int function() {
		if (expect("function") <= 0) return 0;
		if (identifier() <= 0) goto err;
		if (expect("(") <= 0) goto err;
		if (expect(")") <= 0) goto err;
		if (eoltok() <= 0) goto err;
		if (block() <= 0) goto err;
		if (expect("end") <= 0) goto err;
		if (expect("function") <= 0) goto err; // optional
		if (!eoltok() && !eoftok()) goto err;
		return 1;
		err:
		//fprintf(stderr, "function error line %d\n", peek().line);
		doerr("function");
		return -1;
	}

	int block() {
		while (peek().val != "end")
			if   (peek().val == "dim") dim();
			else goto err;
		return 1;
		err:
		//fprintf(stderr, "block error line %d [%s]\n", peek().line, peek().val.c_str());
		doerr("block");
		return -1;
	}

	int dim() {
		if (!expect("dim")) goto err;
		if (!identifier()) goto err;
		if (!eoltok()) goto err;
		return 1;
		err:
		//fprintf(stderr, "dim error line %d\n", peek().line);
		doerr("dim");
		return -1;
	}
};