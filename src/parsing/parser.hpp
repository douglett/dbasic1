#pragma once
#include "../helpers/parsetools.hpp"
#include "../helpers/node.hpp"
#include "tokenizer.hpp"

struct Parser : ParseTools {
	Node ast;
	Tokenizer tok;
	int pos = 0;

	Parser(const Tokenizer& t) { tok = t; }

	int parse() {
		for (auto t : tok.tokens)
			printf("  [%s]\n", t.val.c_str());
		pos = 0;
		int result = prog(ast);
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

	int prog(Node& ast) {
		ast = {"prog"};
//		auto& fn = ast.get("functions");
		while (!eoftok())
			if      (eoltok() > 0) ;
			else if (function(ast) > 0) ;
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

	int function(Node& funclist) {
		if (expect("function") <= 0) return 0;
		Node& func = funclist.push({"function", {
			{"name"},
			{"block"}
		}});
		if (identifier() <= 0) goto err;
		func.get("name").push({ peek(-1).val });
		if (!expect("(") || !expect(")") || !eoltok()) goto err;
		if (block(func.get("block")) <= 0) goto err;
		if (!expect("end") || !expect("function") || !(eoltok() || eoftok())) goto err;
		return 1;
		err:
		doerr("function");
		return -1;
	}

	int block(Node& blk) {
		while (peek().val != "end")
			if   (dim(blk) > 0) ;
			else goto err;
		return 1;
		err:
		doerr("block");
		return -1;
	}

	int dim(Node& blk) {
		if (!expect("dim")) return 0;
		Node& dimn = blk.push({ "dim" });
		if (!identifier()) goto err;
		dimn.push({ "name" }).push({ peek(-1).val });
		if (!eoltok()) goto err;
		return 1;
		err:
		doerr("dim");
		return -1;
	}
};