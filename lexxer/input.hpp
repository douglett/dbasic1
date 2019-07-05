#include <fstream>
#include <sstream>
#include <string>

struct Input_ {
	std::stringstream input;

	int setstr(const std::string& str) {
		input.str(str), input.clear();
		return 0;
	}

	int load(const std::string& fname) {
		std::fstream fs(fname, std::ios::in);
		if (!fs.is_open()) 
			return fprintf(stderr, "error: could not open file '%s'\n", fname.c_str()), 1;
		setstr("");
		input << fs.rdbuf();
		return 0;
	}

	int peek() {
		int i = input.tellg();
		int j = input.peek();
		input.seekg(i);
		return j;
	}

	int get() {
		return input.get();
	}

	int pos() {
		return input.tellg();
	}

	int seek(int p) {
		input.seekg(p);
		return pos();
	}
};

struct Input {
	struct inputpos { int line, pos; };
	std::vector<std::string> lines;
	inputpos curpos = { 0, 0 };

	int load(const std::string& fname) {
		curpos = { 0, 0 };
		std::fstream fs(fname, std::ios::in);
		if (!fs.is_open()) 
			return fprintf(stderr, "error: could not open file '%s'\n", fname.c_str()), 1;
		std::string s;
		while (getline(fs, s))
			lines.push_back(s);
		return 0;
	}

	int peek() {
		if (curpos.line >= (int)lines.size()) return EOF;
		if (curpos.pos >= (int)lines[curpos.line].size()) return '\n';
		return lines[curpos.line][curpos.pos];
	}

	int get() {
		auto c = peek();
		if (c == EOF) return EOF;
		if (c == '\n') return curpos.line++, curpos.pos = 0, '\n';
		return curpos.pos++, c;
	}

	inputpos pos() {
		return curpos;
	}

	void seek(const inputpos& p) {
		curpos = p;
	}

	std::string report() {
		std::stringstream ss;
		ss << "cursor position (" << curpos.line+1 << ", " << curpos.pos+1 << "):" << std::endl;
		ss << "[" << lines[curpos.line] << "]";
		return ss.str();
	}
};