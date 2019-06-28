#include <iostream>
#include "parsing/tokenizer.hpp"

int main() {
	printf("hello world\n");
	Tokenizer tok;
	tok.load("test.bas");
	printf("lines: %d\n", tok.lines.size());
}