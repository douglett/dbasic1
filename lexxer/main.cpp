#include "lexxer.hpp"
#include <iostream>

int main() {
	printf("hello world\n");

	Lexxer l;
	printf("running...\n");
	std::string input = "test  blah";
	int i = l.run("IDENTIFIER", input);
	printf("res: [%d] [%s]\n", i, input.substr(0, i).c_str());
	int j = l.run("WS", input, i);
	i += j;
	j = l.run("IDENTIFIER", input, i);
	printf("res: [%d] [%s]\n", i, input.substr(i, j).c_str());
}