#include "lexxer.hpp"
#include "astparser.hpp"
#include <iostream>

int main() {
	printf("hello world\n");

	ASTParser par;
	par.test();


	// Lexxer l;
	// printf("running...\n");

	// std::string input = "test  123";
	// std::vector<std::string> outp, rules = {
	// 	"IDENTIFIER", "WS", "NUMBER"
	// };

	// int i = 0, j = 0;
	// for (const auto& r : rules) {
	// 	j = l.run(r, input, i);
	// 	auto res = input.substr(i, j > 0 ? j : 0);
	// 	printf("%s: [%d] [%s]\n", r.c_str(), j,res .c_str());
	// 	if (j == -1) break;
	// 	i += j;
	// }
}