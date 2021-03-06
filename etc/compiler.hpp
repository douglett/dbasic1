#pragma once
#include "../helpers/node.hpp"
#include "../helpers/parsetools.hpp"

struct Compiler : ParseTools {
	Node output;

	int build(const Node& ast) {
		try {
			output = module(ast);
		}
		catch (const std::string& err) {
			fprintf(stderr, "compile error: %s\n", err.c_str());
		}
		return 0;
	}

	void error(const std::string& msg, const Node& n) {
		auto msgstr = msg + " (got '" + n.val + "')";
		throw msgstr;
	}

	Node module(const Node& ast) {
		Node p = {"()", {
			{"module"}, {"$dbasic_program"}
		}};
		for (auto& fn : ast.list)
			if   (fn.val == "function") p.push(function(fn));
			else error("unexpected in module", fn);
		return p;
	}

	Node function(const Node& func) {
		auto& name = func.get("name").get(0).val;
		Node fn = {"()", {
			{"func"},
			{"$"+name},
			{"(export \""+name+"\")"},
			{"(result i32)"}
		}};
		locals_inline( func.get("locals"), fn ); // append local variables
		block_inline( func.get("block"), fn ); // append function block
		fn.push({"()", { {"i32.const 0"} }}); // return value
		return fn;
	}

	void locals_inline(const Node& locals, Node& fn) {
		Node local_def, local_set;
		for (auto& l : locals.list) {
			auto& name = l.get("name").get(0).val; // local base name
			local_def.pushs("()").pushs("local $"+name+" i32"); // predefine
			auto ex = expr( l.get("expr") ); // parse defining expression
			local_set.push({"()", { {"set_local $"+name}, ex }}); // set assigning expression to var
		}
		// add locals to function
		fn.append(local_def.list);
		fn.append(local_set.list);
	}

	Node block(const Node& blk) {
		Node b = {"()", { {"block"} }};
		block_inline(blk, b);
		return b;
	}

	void block_inline(const Node& blk, Node& b) {
		for (auto& line : blk.list)
			if      (false) ;
			else if (line.val == "if"    ) b.push( cmdif(line) );
			else if (line.val == "while" ) b.push( cmdwhile(line) );
			else if (line.val == "for"   ) b.push( cmdfor(line) );
			else if (line.val == "return") b.push( cmdreturn(line) );
			else if (line.val == "assign") b.push( cmdassign(line) );
			else    error("unexpected in block", line);
	}

	Node cmdif(const Node& cmd) {
		return {"()", {
			{"if"},
			expr(cmd.get("expr")),
			block(cmd.get("block"))
		}};
	}

	Node cmdwhile(const Node& cmd) {
		std::string lmain = "$loop$0",
			lcont = lmain+"$continue";
		// nested loop
		Node nwhile = {"()", {
			{"block "+lmain}
		}};
		Node& inner = nwhile.push({"()", {
			{"loop "+lcont}
		}});
		// add break condition to inner loop
		inner.push({"()", {
			{"br_if "+lmain},
			expr_negate(cmd.get("expr"))
		}});
		// loop contents
		block_inline( cmd.get("block"), inner );
		// continue loop
		inner.push({"()", { {"br"}, {lcont} }});
		return nwhile;
	}

	Node cmdfor(const Node& cmd) {
		std::string lmain = "$loop$0",
			lcont = lmain+"$continue";
		auto id = cmd.get("id").get(0).val;
		int step = std::stod( cmd.get("step").get(0).val );
		// outer loop
		Node nfor = {"()", {
			{"block "+lmain},
			{"()", { // start values
				{"set_local"},
				{"$"+id},
				var_const( cmd.get("start").get(0).val )
			}},
			{"()", { // inner-loop
				{"loop "+lcont}
			}}
		}};
		// inner-loop
		Node& inner = nfor.get("()", 1);
		// break condition
		inner.push({"()", {
			{"br_if "+lmain},
			{"()", {
				{ step < 0 ? "i32.lt_s" : "i32.gt_s" },
				var_local( id ),
				var_const( cmd.get("end").get(0).val )
			}}
		}});
		// loop contents
		block_inline( cmd.get("block"), inner );
		// step
		inner.push(var_increment( id, cmd.get("step").get(0).val ));
		// do loop
		inner.push({"()", { {"br"}, {lcont} }});
		return nfor;
	}

	Node cmdreturn(const Node& cmd) {
		return {"()", {
			{"return"},
			expr(cmd.get("expr"))
		}};
	}

	Node cmdassign(const Node& cmd) {
		auto id = cmd.get("id").get(0).val;
		return {"()", {
			{"set_local $"+id},
			expr(cmd.get("expr"))
		}};
	}

	Node expr(const Node& ex) {
		return expr_inner(ex.get(0));
	}
	Node expr_inner(const Node& ex) {
		auto& val = ex.val;
		if (isnumber(val)) return var_const(val);
		if (isidentifier(val)) return var_local(val);
		std::string op;
		if      (val == "==") op = "i32.eq";
		else if (val == ">" ) op = "i32.gt_s";
		else if (val == ">=") op = "i32.ge_s";
		else if (val == "<" ) op = "i32.lt_s";
		else if (val == "<=") op = "i32.le_s";
		else if (val == "+" ) op = "i32.add";
		else if (val == "-" ) op = "i32.sub";
		else if (val == "*" ) op = "i32.mul";
		else if (val == "/" ) op = "i32.div";
		else    error("unexpected in expression", ex);
		return {"()", { {op}, expr_inner(ex.get(0)), expr_inner(ex.get(1)) }};
	};

	/** helpers **/

	// basic lists
	Node var_const(const std::string& var) {
		return {"()", { {"i32.const"}, {var} }};
	}
	Node var_local(const std::string& id) {
		return {"()", { {"get_local"}, {"$"+id} }};
	}

	// negate expression
	Node expr_negate(const Node& ex) {
		return {"()", {
			{"i32.eq"},
			{"(i32.const 0)"},
			expr( ex ) // true expression
		}};
	}

	// plus-equals
	Node var_increment(const std::string& id, const std::string num="1") {
		return {"()", {
			{"set_local $"+id},
			{"()", {
				{"i32.add"},
				{"(get_local $"+id+")"},
				{"(i32.const "+num+")"}
			}}
		}};
	}


	// int in_list(const std::vector<std::string>>& list, const std::string& val) {
	// 	for (auto& l : list)
	// 		if (l == val) return 1;
	// 	return 0;
	// }
};