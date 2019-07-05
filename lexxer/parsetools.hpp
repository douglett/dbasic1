#pragma once
#include <string>
#include <vector>
#include <iostream>

struct ParseTools {
	typedef std::vector<std::string> vecstr;

	struct Node {
		std::string val;
		std::vector<Node> list;

		Node& find(const std::string& id) {
			for (auto& n : list)
				if (n.val == id) return n;
			throw std::string("missing node: "+id);
		}
		const Node& find(const std::string& id) const {
			for (auto& n : list)
				if (n.val == id) return n;
			throw std::string("missing node: "+id);
		}
	};

	static void shownode(const Node& n, int ind=0) {
		printf("%s[%s]\n", std::string(ind*2, ' ').c_str(), n.val.c_str());
		for (auto& nn : n.list)
			shownode(nn, ind+1);
	}
	static std::string showlisp(const Node& n, int ind=0) {
		auto is = std::string(ind*2, ' ');
		std::string s;
		if (n.val == "()") {
			s = is + "(\n";
			for (auto& nn : n.list)
				s += showlisp(nn, ind+1);
			s += is + ")\n";
		}
		else
			s = is + n.val + "\n";
		return s;
	}

	static int isletter(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}
	static int isdigit(char c) {
		return c >= '0' && c <= '9';
	}
	static int isspecial(char c) {
		return !isletter(c) && !isdigit(c) && !isspace(c);
	}
	static int iswspace(char c) {
		return c == ' ' || c == '\t';
	}
	static int iseol(char c) {
		return c == '\n' || c == EOF;
	}
	static int iseof(char c) {
		return c == EOF;
	}
	static int iseof(const std::string& s, int pos) {
		return pos >= (int)s.length();
	}

	static int inlist(const std::string& s, const vecstr& vs) {
		for (auto& v : vs)
			if (s == v) return 1;
		return 0;
	}

	static vecstr split(const std::string& str) {
		vecstr vs;
		std::string s;
		for (auto c : str) {
			if (isspace(c) || isspecial(c)) {
				if (s != "") vs.push_back(s), s = "";
				if (isspecial(c)) vs.push_back(std::string() + c);
			}
			else
				s += c;
		}
		if (s != "") vs.push_back(s), s = "";
		return vs;
	}
};