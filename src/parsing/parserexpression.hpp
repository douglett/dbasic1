#pragma once
#include "parserbase.hpp"

struct ParserExpression : ParserBase {

	int expr(Node& expr) {
		return expr_add(expr);
	}

	int expr_add(Node& expr) {
		int res = expr_mul(expr);
		if (res < 1) return res;
		if (expect("+") || expect("-")) {
			expr = {peek(-1).val, { expr, {} }};
			return expr_mul(expr.get(1));
		}
		return 1;
	}

	int expr_mul(Node& expr) {
		int res = expr_atom(expr);
		if (res < 1) return res;
		if (expect("*") || expect("/")) {
			expr = {peek(-1).val, { expr, {} }};
			return expr_atom(expr.get(1));
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