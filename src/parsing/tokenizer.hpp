#include <string>
#include <vector>
#include <fstream>

struct Token {
	std::string val;
	int line;
};

struct Tokenizer {
	std::vector<std::string> lines;
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

	int parse() {
		int lno = 0;
		for (const auto& line : lines) {
			lno++;
			std::string s;
			for (const auto& c : line) {
				if (c == ' ' || c == '\t') {
					if (s.length()) list.push_back({ s, lno }), s = "";
				}
				else {
					s += 
				}
			}
		}
	}
};