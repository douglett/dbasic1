#pragma once
#include <string>
#include <vector>
#include <iostream>

struct ParserTools {
	typedef std::vector<std::string> vecstr;

//	struct Node {
//		std::string val;
//		std::vector<Node> list;
//	};
//
//	static void shownode(const Node& n, int ind=0) {
//		printf("%s[%s]\n", std::string(ind*2, ' ').c_str(), n.val.c_str());
//		for (auto& nn : n.list)
//			shownode(nn, ind+1);
//	}

	static int isletter(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}
	static int isdigit(char c) {
		return c >= '0' && c <= '9';
	}
	static int iswhitespace(char c) {
		return c == ' ' || c == '\t';
	}
	static int isspecial(char c) {
		return !isletter(c) && !isdigit(c) && !iswhitespace(c);
	}
	static int iseol(char c) {
		return c == '\n' || c == '\r' || c == '\f';
	}

	static int isidentifier(const std::string& str) {
		int start = 1;
		for (char c : str)
			if (start) { if (!isletter(c)) return 0; start = 0; }
			else if (!isletter(c) && !isdigit(c)) return 0;
		return 1;
	}
	static int isnumber(const std::string& str) {
		for (char c : str)
			if (!isdigit(c)) return 0;
		return 1;
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