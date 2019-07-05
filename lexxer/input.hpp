#include <fstream>
#include <sstream>
#include <string>

struct Input {
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