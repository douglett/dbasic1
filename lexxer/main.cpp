#include "lexxer.hpp"
#include <iostream>

int main() {
	printf("hello world\n");

	Lexxer l;
//	auto vs = l.split(l.identifier);
//	for (auto s : vs)
//		printf("[%s]\n", s.c_str());

	l.mkrule();
	printf("%s\n", l.identifier.c_str());
	l.shownode(l.rule);
}
