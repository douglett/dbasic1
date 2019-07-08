#pragma once
#include "parserbase.hpp"
#include "parserexpression.hpp"

struct Parser : ParserExpression {
	Node ast;

	Parser(const Tokenizer& t) { tok = t; }

	int parse() {
		// for (auto t : tok.tokens)
		// 	printf("  [%s]\n", t.val.c_str());
		pos = 0;
		int result = prog(ast);
		printf("parse result: %d\n", result);
		return result;
	}

	int prog(Node& ast) {
		ast = {"prog"};
		while (!eoftok())
			if      (lineend() > 0) ;
			else if (function(ast) > 0) ;
			else    return doerr("root-scope");
		return 1;
	}

	int function(Node& funclist) {
		if (expect("function") <= 0) return 0;
		Node& func = funclist.push({"function", { // shouldn't reflow unless funclist is altered.
			{"name"},
			{"locals"},
			{"block"}
		}});
		if (identifier() <= 0) goto err; // function name
		func.get("name").pushs(peek(-1).val); // save function name
		if (!expect("(") || !expect(")") || !lineend()) goto err; // args (TEMP)
		if (locals(func.get("locals")) < 0) goto err; // local dims (can be none)
		if (block(func.get("block")) <= 0) goto err; // main function block (can be empty)
		if (!expect("end") || !expect("function") || !lineend()) goto err; // function end
		return 1;
		err:
		doerr("function");
		return -1;
	}

	int locals(Node& locals) {
		while (true) {
			int res = dim(locals);
			if (res == -1) goto err; // error in dim
			if (res == 0) break; // no more dims
		}
		return !!locals.list.size(); // 1 or 0
		err:
		doerr("function-locals");
		return -1;
	}

	int dim(Node& blk) {
		if (!expect("dim")) return 0;
		Node& dim = blk.push({"dim", {
			{"name"},
			{"expr"}
		}});
		if (!identifier()) goto err; // dim name
		dim.get("name").pushs(peek(-1).val); // save name
		// initial value expression
		if (expect("=")) {
			if (expr( dim.get("expr").pushs("??") ) < 1) goto err;
		}
		else
			dim.get("expr").pushs("0"); // assign default value (0)
		if (!lineend()) goto err; // end of line
		return 1;
		err:
		doerr("dim");
		return -1;
	}

	int block(Node& blk) {
		while (peek().val != "end")
			if      (lineend()) ;
			else if (cmdif(blk)) ;
			else if (cmdwhile(blk)) ;
			else if (cmdret(blk)) ;
			else    goto err; // unexpected in block
		return 1;
		err:
		doerr("block");
		return -1;
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
		doerr("if");
		return -1;
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
};