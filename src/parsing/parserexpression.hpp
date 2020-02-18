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
		if      (acceptm({">", "="}) || acceptm({"<", "="})) op = peek(-2).val + "=";
		else if (acceptm({"=", "="}) || acceptm({"!", "="})) op = peek(-2).val + "=";
		else if (expect(">") || expect("<")) op = peek(-1).val;
		else    return 1;
		ex = {"operator", op, { ex, {"??"} }};
		return expr_add(ex.children[1]);
	}

	int expr_add(ASTnode& ex) {
		if (!expr_mul(ex)) return 0;
		while (expect("+") || expect("-")) {
			ex = {"operator", peek(-1).val, { ex, {"??"} }};
			if (!expr_mul(ex.children[1])) return 0;
		}
		return 1;
	}

	int expr_mul(ASTnode& ex) {
		if (!expr_atom(ex)) return 0;
		while (expect("*") || expect("/")) {
			ex = {"operator", peek(-1).val, { ex, {"??"} }};
			if (!expr_atom(ex.children[1])) return 0;
		}
		return 1;
	}

	int expr_atom(ASTnode& ex) {
		if      (identifier()) ex = { "identifier", peek(-1).val };
		else if (number()) ex = { "number", peek(-1).val };
		else if (peek().val == "(") return expr_brackets(ex);
		else    return 0;
		return 1;
	}

	int expr_brackets(ASTnode& ex) {
		if (!expect("(") || expr_top(ex) < 1 || !expect(")")) return -1;
		ex = {"()", "", { ex }};
		return 1;
	}
};