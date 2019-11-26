#pragma once
#include "parserbase.hpp"
#include "parserexpression.hpp"
#include "../helpers/astnode.hpp"

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
		// Node ast = {"prog"};
		while (!eoftok())
			if      (lineend() > 0) ;
			// globals here
			else if (function(prog) > 0) ;
			else    return doerr("root-scope");
		return 1;
	}

	int function(ASTnode& prog) {
		if (expect("function") <= 0) return 0;
		
		ASTnode& func2 = prog.push({ "function", "", {
			{"name"},
			{"locals"},
			{"block"}
		}});

		Node funclist;
		Node& func = funclist.push({"function", { // shouldn't reflow unless funclist is altered.
			{"name"},
			{"locals"},
			{"block"}
		}});
		if (identifier() <= 0) goto err; // function name
		// func.get("name").pushs(peeks(-1)); // save function name
		func2.value = func2.find("name").at(0)->value = peeks(-1);
		if (!expect("(") || !expect(")") || !lineend()) goto err; // args (TEMP)
		if (locals(func.get("locals")) < 0) goto err; // local dims (can be none)
		if (block(func.get("block")) <= 0) goto err; // main function block (can be empty)
		if (!expect("end") || !expect("function") || !lineend()) goto err; // function end
		return 1;
		err:
		return doerr("function");
	}

	int locals(Node& locals) {
		while (true) {
			int res = dim(locals);
			if (res == -1) goto err; // error in dim
			if (res == 0) break; // no more dims
		}
		return !!locals.list.size(); // 1 or 0
		err:
		return doerr("function-locals");
	}

	int dim(Node& blk) {
		if (!expect("dim")) return 0;
		Node& dim = blk.push({"dim", {
			{"name"},
			{"expr"}
		}});
		if (!identifier()) goto err; // dim name
		dim.get("name").pushs(peeks(-1)); // save name
		// initial value expression
		if (expect("=")) {
			if (expr( dim.get("expr").pushs("??") ) < 1) goto err;
		}
		else
			dim.get("expr").pushs("0"); // assign default value (0)
		if (!lineend()) goto err; // end of line
		return 1;
		err:
		return doerr("dim");
	}

	int block(Node& blk) {
		while (!(peeks() == "end" || peeks() == "next"))
			if      (lineend()) ;
			else if (cmdif(blk) == 1) ;
			else if (cmdwhile(blk) == 1) ;
			else if (cmdfor(blk) == 1) ;
			else if (cmdret(blk) == 1) ;
			else if (cmdassign(blk) == 1) ;
			else    goto err; // unexpected in block
		return 1;
		err:
		return doerr("block");
	}

	int cmdif(Node& blk) {
		if (!expect("if")) return 0;
		Node& cmd = blk.push({"if", {
			{"expr"},
			{"block"}
		}});
		auto& ex = cmd.get("expr").pushs("??");
		if (expr(ex) < 1 || !expect("then") || !lineend()) goto err;
		if (block(cmd.get("block")) < 1) goto err;
		if (!expect("end") || !expect("if") || !lineend()) goto err;
		return 1;
		err:
		return doerr("if");
	}

	int cmdwhile(Node& blk) {
		if (!expect("while")) return 0;
		Node& cmd = blk.push({"while", {
			{"expr"},
			{"block"}
		}});
		int ok = 
			expr( cmd.get("expr").pushs("??") ) == 1 
			&& expect("do") 
			&& lineend()
			&& block( cmd.get("block") ) == 1
			&& expectm({ "end", "while" }) 
			&& lineend();
		return ok ? 1 : doerr("if");
	}

	int cmdfor(Node& blk) {
		if (!expect("for")) return 0;
		Node& cmd = blk.push({"for", {
			{"id"},
			{"start"},
			{"end"},
			{"step"},
			{"block"}
		}});
		std::string id;
		// for conditions
		if (!identifier()) goto err;
		id = peeks(-1);
		cmd.get("id").pushs(id);
		if (!expect("=") || !number()) goto err;
		cmd.get("start").pushs(peeks(-1));
		if (!expect("to") || !number()) goto err;
		cmd.get("end").pushs(peeks(-1));
		// optional step
		if (expect("step")) {
			if (!number()) goto err;
			cmd.get("step").pushs(peeks(-1));
		} else {
			cmd.get("step").pushs("1"); // default: add 1 per iteration
		}
		// contents
		if (block( cmd.get("block") ) < 1) goto err;
		// next - make sure identifiers match
		if (!expect("next") || peeks() != id || !identifier()) goto err;
		return 1;
		err:
		return doerr("for");
	}

	int cmdret(Node& blk) {
		if (!expect("return")) return 0;
		Node& cmd = blk.pushs("return");
		auto& ex = cmd.pushs("expr").pushs("0"); // default return
		if (lineend()) return 1;
		if (expr(ex) < 1 || !lineend()) goto err;
		return 1;
		err:
		return doerr("return"), -1;
	}

	int cmdassign(Node& blk) {
		// int p = pos;
		if (!identifier()) return 0;
		auto& id = peeks(-1);
		Node& cmd = blk.push({"assign", {
			{"id", { {id} }},
			{"expr"}
		}});
		Node& ex = cmd.get("expr").pushs("??");
		// normal set
		if (expect("=")) {
		 	if (expr(ex) < 1) goto err;
		}
		// increment / decrement
		else if (
				acceptm({"+", "+"})
				|| acceptm({"-", "-"}) ) {
			ex = {peeks(-1), { {id}, {"1"} }};
		}
		// modify-equals
		else if (
				acceptm({"+", "="})
				|| acceptm({"-", "="})
				|| acceptm({"-", "="})
				|| acceptm({"-", "="}) ) {
			ex = {peeks(-2), { {id}, {"??"} }};
			if (expr(ex.get(1)) < 1) goto err;
		}
		// unknown
		else goto err;
		if (!lineend()) goto err; // eol
		return 1;
		err:
		return doerr("assign");
	}
};