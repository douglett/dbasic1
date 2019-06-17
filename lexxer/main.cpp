#include "lexxer.hpp"
#include <iostream>

int main() {
	printf("hello world\n");

	Lexxer l;
	printf("running...\n");
	l.run(l.rules[0], "test");
}