#pragma once
#include "parserbase.hpp"

struct ParserExpression : ParserBase {

	int expr(Node& ex) {
		return expr_eq(ex);
	}

	int expr_eq(Node& ex) {
		if (!expr_add(ex)) return 0;
		std::string op;
		if      (acceptm({">", "="}) || acceptm({"<", "="})) op = peek(-2).val + "=";
		else if (acceptm({"=", "="}) || acceptm({"!", "="})) op = peek(-2).val + "=";
		else if (expect(">") || expect("<")) op = peek(-1).val;
		else    return 1;
		ex = {op, { ex, {} }};
		return expr_add(ex.get(1));
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

	int expr_atom(Node& ex) {
		if      (identifier()) ex = { peek(-1).val };
		else if (number()) ex = { peek(-1).val };
		else if (peek().val == "(") return expr_brackets(ex);
		else    return 0;
		return 1;
	}

	int expr_brackets(Node& ex) {
		if (!expect("(") || expr(ex) < 1 || !expect(")")) return -1;
		ex = {"()", { ex }};
		return 1;
	}
};