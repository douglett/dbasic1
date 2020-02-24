#pragma once
#include "parserbase.hpp"

struct ParserExpression : ParserBase {

	int expr(ASTnode& ex) {
		if (ex.type != "expr") throw std::string("expected expr node");
		ex.children = { {"??"} }; // put expression here (default unusable value)
		return expr_top(ex.children[0]);
	}

	int expr_top(ASTnode& ex) {
		return expr_eq(ex);
	}

	int expr_eq(ASTnode& ex) {
		if (!expr_add(ex)) return 0;
		std::string op;
		if      (acceptm({">", "="}) || acceptm({"<", "="})) op = peeks(-2) + "=";
		else if (acceptm({"=", "="}) || acceptm({"!", "="})) op = peeks(-2) + "=";
		else if (expect(">") || expect("<")) op = peeks(-1);
		else    return 1;
		ex = {"operator", op, { ex, {"??"} }};
		return expr_add(ex.children[1]);
	}

	int expr_add(ASTnode& ex) {
		if (!expr_mul(ex)) return 0;
		while (expect("+") || expect("-")) {
			ex = {"operator", peeks(-1), { ex, {"??"} }};
			if (!expr_mul(ex.children.at(1))) return 0;
		}
		return 1;
	}

	int expr_mul(ASTnode& ex) {
		if (!expr_atom(ex)) return 0;
		while (expect("*") || expect("/")) {
			ex = {"operator", peeks(-1), { ex, {"??"} }};
			if (!expr_atom(ex.children.at(1))) return 0;
		}
		return 1;
	}

	int expr_atom(ASTnode& ex) {
		int res = 0;
		if      ((res = expr_call(ex)) != 0) return res; // function call
		else if ((res = expr_brackets(ex) != 0)) return res; // brackets
		else if (identifier()) ex = { "identifier", peeks(-1) }; // normal identifier
		else if (number()) ex = { "number", peeks(-1) }; // number
		else    return 0;
		return 1;
	}

	int expr_call(ASTnode& ex) {
		const int p = pos;
		if (!identifier() || !expect("(")) return pos = p, 0;
		ex = { "call", peeks(-2) }; // function call
		if (!expect(")")) return -1;
		return 1;
	}

	int expr_brackets(ASTnode& ex) {
		if (!expect("(")) return 0;
		ex = {"()", "", { {"??"} }};
		if (expr_top(ex.children.at(0)) < 1 || !expect(")")) return -1;
		return 1;
	}
};