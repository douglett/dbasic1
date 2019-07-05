#pragma once
#include "../helpers/node.hpp"
#include "../helpers/parsetools.hpp"

struct Compiler : ParseTools {
	Node output;

	int build(const Node& ast) {
		try {
			output = prog(ast);
		}
		catch (const std::string& err) {
			fprintf(stderr, "compile error: %s\n", err.c_str());
		}
		return 0;
	}

	Node prog(const Node& ast) {
		Node p = {"()", {
			{"module"}, {"$dbasic_program"}
		}};
		for (auto& fn : ast.list)
			if   (fn.val == "function") p.push(function(fn));
			else throw std::string("expected function");
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
		locals(func.get("locals"), fn); // local variables
		fn.push( block(func.get("block")) ); // function block
		fn.push({"()", { {"i32.const 0"} }}); // return value
		return fn;
	}

	void locals(const Node& locals, Node& fn) {
		Node local_def, local_set;
		for (auto& l : locals.list) {
			auto& name = l.get("name").get(0).val; // local base name
			local_def.pushs("()").pushs("local $"+name+" i32"); // predefine
			auto ex = expr( l.get("expr").get(0) ); // parse defining expression
			local_set.push({"()", { {"set_local $"+name}, ex }}); // set assigning expression to var
		}
		// add locals to function
		fn.append(local_def.list);
		fn.append(local_set.list);
	}

	Node block(const Node& block) {
		Node b = {"()", { {"block"} }};
		for (auto& line : block.list)
			if      (false) ;
			else if (line.val == "if") b.push(cmdif(line));
			else if (line.val == "return") b.push(cmdreturn(line));
			else    throw std::string("unexpected in block");
		return b;
	}

	Node cmdif(const Node& cmd) {
		return {"()", {
			{"if"},
			expr(cmd.get("expr").get(0)),
			block(cmd.get("block"))
		}};
	}

	Node cmdreturn(const Node& cmd) {
		return {"()", {
			{"return"},
			expr(cmd.get("expr").get(0))
		}};
	}

	Node expr(const Node& ex) {
		auto& val = ex.val;
		if (isnumber(val)) return { "(i32.const "+val+")" };
		if (isidentifier(val)) return { "(get_local $"+val+")" };
		if (val == "==") return {"()", { {"i32.eq" }, expr(ex.get(0)), expr(ex.get(1)) }};
		if (val == "+" ) return {"()", { {"i32.add"}, expr(ex.get(0)), expr(ex.get(1)) }};
		if (val == "-" ) return {"()", { {"i32.sub"}, expr(ex.get(0)), expr(ex.get(1)) }};
		if (val == "*" ) return {"()", { {"i32.mul"}, expr(ex.get(0)), expr(ex.get(1)) }};
		if (val == "/" ) return {"()", { {"i32.div"}, expr(ex.get(0)), expr(ex.get(1)) }};
		throw std::string("unexpected in expression");
	};

//	int in_list(const std::vector<std::string>>& list, const std::string& val) {
//		for (auto& l : list)
//			if (l == val) return 1;
//		return 0;
//	}

//	Node dim(const Node& d) {
//		auto& name = d.get("name").get(0).val;
//		return {"()", { {"local $"+name+" i32"} }};
//	}
};