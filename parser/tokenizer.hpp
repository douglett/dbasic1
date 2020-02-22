#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "parsetools.hpp"

struct Token {
	std::string val;
	int line;
};

struct Tokenizer : ParseTools {
	std::vector<std::string> lines;
	std::vector<Token> tokens;
	int line = 0;

	int load(const std::string& fname) {
		std::fstream fs(fname, std::ios::in);
		if (!fs.is_open())
			return fprintf(stderr, "could not open file: %s\n", fname.c_str()), 1;
		std::string s;
		while (getline(fs, s))
			lines.push_back(s);
		return 0;
	}

	void pushs(std::string& s, int lno) {
		if (s.length())
			tokens.push_back({ s, lno }), s = "";
	}

	int parse() {
		int lno = 0;
		for (const auto& line : lines) {
			lno++;
			std::string s;
			for (unsigned int i = 0; i < line.length(); i++) {
				auto c = line[i];
				// comments
				if (c == '\'') {
					pushs(s, lno);
					tokens.push_back({ line.substr(i), lno });
					break;
				// word breaks
				} else if (iswhitespace(c)) {
					pushs(s, lno);
				// control characters
				} else if (isspecial(c)) {
					pushs(s, lno);
					if (isspecial(c)) 
						tokens.push_back({ std::string()+c, lno });
				// add to word
				} else {
					s += c;
				}
			}
			if (s.length()) tokens.push_back({ s, lno });
			tokens.push_back({ "[EOL]", lno });
		}
		if (tokens.size()) tokens.pop_back();
		tokens.push_back({ "[EOF]", lno });
		return 0;
	}

	std::string show() {
		std::string s;
		for (const auto& t : tokens)
			s += "\n  [" + t.val + "]";
		return s;
	}
};