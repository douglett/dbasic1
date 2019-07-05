#pragma once
#include "parserbase.hpp"

struct ParserExpression : ParserBase {

	int expr(Node& ex) {
		return expr_eq(ex);
	}

	int expr_eq(Node& ex) {
		if (!expr_add(ex)) return 0;
		if ((expect("=") || expect("!")) && expect("=")) {
			ex = {peek(-2).val+"=", { ex, {} }};
			return expr_add(ex.get(1));
		}
		return 1;
	}

	int expr_add(Node& ex) {
		if (!expr_mul(ex)) return 0;
		while (expect("+") || expect("-")) {
			ex = {peek(-1).val, { ex, {} }};
			if (!expr_mul(ex.get(1))) return 0;
		}
		return 1;
	}

	int expr_mul(Node& ex) {
		if (!expr_atom(ex)) return 0;
		while (expect("*") || expect("/")) {
			ex = {peek(-1).val, { ex, {} }};
			if (!expr_atom(ex.get(1))) return 0;
		}
		return 1;
	}

	int expr_atom(Node& expr) {
		if      (identifier()) expr = { peek(-1).val };
		else if (number()) expr = { peek(-1).val };
		else if (peek().val == "(") return expr_brackets(expr);
		else    return 0;
		return 1;
	}

	int expr_brackets(Node& expr) {
		if (!expect("(") || this->expr(expr) < 1 || !expect(")")) return -1;
		expr = {"()", { expr }};
		return 1;
	}
};