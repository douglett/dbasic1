#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "parsertools.hpp"

struct Token {
	std::string val;
	int line;
};

struct Tokenizer : ParserTools {
	std::string fname;
	std::vector<std::string> lines;
	std::vector<Token> tokens;
	Token eof_tok;
	int line = 0;

	int load(const std::string& fname) {
		this->fname = fname;
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

	const Token& get(int pos) {
		if (pos < 0 || pos >= (int)tokens.size())
			return eof_tok = { "[EOF]", (int)lines.size() };
		return tokens.at(pos);
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
		if (tokens.size()) tokens.pop_back(); // pop last EOL, which was automatically added
		// tokens.push_back({ "[EOF]", lno });
		return 0;
	}

	std::string show() {
		std::string s;
		for (const auto& t : tokens)
			s = s + (s.length() ? "\n" : "") + "  [" + t.val + "]";
		return s;
	}
};