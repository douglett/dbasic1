#include "lexxer.hpp"
#include <iostream>

int main() {
	printf("hello world\n");

	Lexxer l;
	printf("running...\n");
	int i = l.run("IDENTIFIER", "test");
	printf("res: %d\n", i);
}