#pragma once
#include "parserexpression.hpp"

struct Parser : ParserExpression {
	ASTnode ast;

	Parser(const Tokenizer& t) { tok = t; }

	int parse() {
		pos = 0;
		int result = prog(ast);
		// printf("parse result: %d\n", result);
		return result;
	}

	int prog(ASTnode& prog) {
		prog = { type: "prog" };
		while (!eoftok())
			if      (lineend() > 0) ;
			// globals here
			else if (function(prog) > 0) ;
			else    return doerr("root-scope");
		return 1;
	}

	int function(ASTnode& prog) {
		if (expect("function") <= 0) return 0;
		ASTnode& func = prog.push({ "function", "", { // build function container
			// {"name"},
			{"locals"},
			{"block"}
		}});
		if (identifier() <= 0) goto err; // function name
		// func.value = func.get("name").value = peeks(-1); // save function name
		func.value = peeks(-1); // save function name
		if (!expect("(") || !expect(")") || !lineend()) goto err; // args (TEMP)
		if (locals(func.get("locals")) < 0) goto err; // local dims (can be none)
		if (block(func.get("block")) <= 0) goto err; // main function block (can be empty)
		if (!expect("end") || !expect("function") || !lineend()) goto err; // function end
		return 1;
		err:
		return doerr("function");
	}

	int locals(ASTnode& locals) {
		while (true) {
			int res = dim(locals);
			if (res == -1) goto err; // error in dim
			if (res == 0) break; // no more dims
		}
		return locals.children.size() > 0; // 1 or 0
		err:
		return doerr("function-locals");
	}

	int dim(ASTnode& blk) {
		if (!expect("dim")) return 0;
		ASTnode& dim = blk.push({"dim", "", {
			// {"name"},
			{"expr"}
		}});
		if (!identifier()) goto err; // dim name
		// dim.value = dim.get("name").value = peeks(-1); // save name
		dim.value = peeks(-1); // save name
		// initial value expression
		if (!expect("="))
			dim.get("expr").push({ "number", "0" }); // assign default value (0)
		else if (expr(dim.get("expr")) < 1)
			goto err;
		if (!lineend()) goto err; // end of line
		return 1;
		err:
		return doerr("dim");
	}

	int block(ASTnode& blk) {
		while (!(peeks() == "end" || peeks() == "next"))
			if      (lineend()) ;
			else if (cmdif(blk) == 1) ;
			else if (cmdwhile(blk) == 1) ;
			else if (cmdfor(blk) == 1) ;
			else if (cmdret(blk) == 1) ;
			else if (cmdassign(blk) == 1) ;
			else if (cmdexpr(blk) == 1) ;
			else    goto err; // unexpected in block
		return 1;
		err:
		return doerr("block");
	}

	int cmdif(ASTnode& blk) {
		if (!expect("if")) return 0;
		ASTnode& cmd = blk.push({"if", "", {
			{"expr"},
			{"block"}
		}});
		if (expr(cmd.get("expr")) < 1 || !expect("then") || !lineend()) goto err;
		if (block(cmd.get("block")) < 1) goto err;
		if (!expect("end") || !expect("if") || !lineend()) goto err;
		return 1;
		err:
		return doerr("if");
	}

	int cmdwhile(ASTnode& blk) {
		if (!expect("while")) return 0;
		ASTnode& cmd = blk.push({"while", "", {
			{"expr"},
			{"block"}
		}});
		int ok =
			expr( cmd.get("expr") ) == 1
			&& expect("do")
			&& lineend()
			&& block( cmd.get("block") ) == 1
			&& expectm({ "end", "while" })
			&& lineend();
		return ok ? 1 : doerr("if");
	}

	int cmdfor(ASTnode& blk) {
		if (!expect("for")) return 0;
		ASTnode& cmd = blk.push({"for", "", {
			{"identifier"},
			{"start"},
			{"end"},
			{"step"},
			{"block"}
		}});
		std::string id;
		// for conditions
		if (!identifier()) goto err;
		id = peeks(-1);
		cmd.get("identifier").value = id;
		if (!expect("=") || !number()) goto err;
		cmd.get("start").push({ "number", peeks(-1) });
		if (!expect("to") || !number()) goto err;
		cmd.get("end").push({ "number", peeks(-1) });
		// optional step
		if (expect("step")) {
			if (!number()) goto err;
			cmd.get("step").push({ "number", peeks(-1) });
		} else {
			cmd.get("step").push({ "number", "1" }); // default: add 1 per iteration
		}
		// contents
		if (block( cmd.get("block") ) < 1) goto err;
		// next - make sure identifiers match
		if (!expect("next") || peeks() != id || !identifier()) goto err;
		return 1;
		err:
		return doerr("for");
	}

	int cmdret(ASTnode& blk) {
		if (!expect("return")) return 0;
		ASTnode& cmd = blk.push({ "return", "", {
			{"expr"}
		}});
		if (lineend()) return cmd.get("expr").push({ "number", "0" }), 1; // default return (0)
		if (expr(cmd.get("expr")) < 1 || !lineend()) goto err; // parse return expression
		return 1;
		err:
		return doerr("return"), -1;
	}

	int cmdassign(ASTnode& blk) {
		const int p = pos;
		if (!identifier()) return 0;
		auto& id = peeks(-1);
		ASTnode& cmd = blk.push({"assign", "", {
			{"identifier", id},
			{"expr"}
		}});
		// normal set
		if (expect("=")) {
		 	if (expr(cmd.get("expr")) < 1) goto err;
		}
		// increment / decrement
		else if (
				acceptm({"+", "+"})
				|| acceptm({"-", "-"}) ) {
			cmd.get("expr").push({"operator", "+", {
				{"identifier", id},
				{"number", "1"}
			}});
		}
		// modify-equals
		else if (
				acceptm({"+", "="})
				|| acceptm({"-", "="})
				|| acceptm({"*", "="})
				|| acceptm({"/", "="}) ) {
			// parse temporary expression
			auto op = peeks(-2);
			ASTnode ex = {"expr"};
			if (expr(ex) < 1) goto err;
			// append to += expression
			cmd.get("expr").push({ "operator", op, {
				{"identifier", id},
				ex.children.at(0)
			}});
		}
		// not assignment
		else {
			blk.pop();
			return pos = p, 0;
		}
		if (!lineend()) goto err; // eol
		return 1;
		err:
		return doerr("assign");
	}

	int cmdexpr(ASTnode& blk) {
		auto& ex = blk.push({ "expr" });
		int res = expr(ex);
		if (res  < 0) goto err;
		if (res == 0) return blk.pop(), 0;
		return res;
		err:
		return doerr("cmdexpr");
	}
};