#pragma once
#include "parserexpression.hpp"

struct Parser : ParserExpression {
	ASTnode ast;

	Parser(const Tokenizer& t) { tok = t; }

	int parse() {
		pos = 0;
		int result = prog(ast);
		// printf("parse result: %d\n", result);
		return result != 1; // returns 1 on error to outside
	}

	int prog(ASTnode& prog) {
		prog = { type: "prog", value: tok.fname };
		int isfunc = 0;
		while (!eoftok())
			if      (lineend() > 0) ;
			else if (isfunc && peeks() == "dim") return doerr("dim after function block");
			else if (dim(prog) > 0) ; // globals
			else if (function(prog) > 0) isfunc = 1; // functions - begin function block
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
		int ok =
			identifier() // function name
			&& (func.value = peeks(-1), 1) // save function name
			&& expectm({ "(", ")" }) // args (TEMP)
			&& lineend()
			&& locals( func.get("locals") ) >= 0 // locals (can be nothing)
			&& block( func.get("block") ) == 1 // main function block
			&& expectm({ "end", "function" })
			&& lineend();
		return ok ? 1 : doerr("function");
	}

//	int globals(ASTnode& globals) {
//		int res = locals(globals);
//		if (res == -1) return doerr("globals");
//		return res;
//	}

	int locals(ASTnode& locals) {
		while (true) {
			int res = dim(locals);
			if (res == -1) return doerr("function-locals"); // error in dim
			if (res ==  0) break; // no more dims
		}
		return locals.children.size() > 0; // 1 or 0
	}

	int dim(ASTnode& blk) {
		if (!expect("dim")) return 0;
		ASTnode& dim = blk.push({"dim", "", {
			// {"name"},
			{"expr"}
		}});
		int ok =
			identifier()
			&& (dim.value = peeks(-1), 1) // save variable name
			&& (
				(expect("=") && expr( dim.get("expr") )) // assignment expression
				|| (dim.get("expr").push({ "number", "0" }), 1) // assign default value (0)
			)
			&& lineend();
		return ok ? 1 : doerr("dim");
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
			// else if (cmdcall(blk) == 1) ;
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
		int ok =
			expr( cmd.get("expr") ) == 1
			&& expect("then")
			&& lineend()
			&& block( cmd.get("block") ) == 1
			&& expectm({ "end", "if" })
			&& lineend();
		return ok ? 1 : doerr("if");
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
		return doerr("return");
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
		if (res  < 0) return doerr("cmdexpr");
		if (res == 0) blk.pop();
		return res;
	}

	// int cmdcall(ASTnode& blk) {
	// 	auto& ex = blk.push({ "??" });
	// 	int res = expr_call(ex);
	// 	if (res  < 0) goto err;
	// 	if (res == 0) return blk.pop(), 0;
	// 	return res;
	// 	err:
	// 	return doerr("cmdcall");
	// }
};