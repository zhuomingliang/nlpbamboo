#include "bamboo.hxx"
#include <stdio.h>

int main(int argc, char *argv[])
{
	size_t i;
	struct token_queue *q;
	if (argc < 2)
		return 0;

	void *handle = bamboo_init("");
	size_t n = bamboo_parse_more(handle, &q, argv[1]);

	for (i = 0; i < n; i++) {
		printf("%s\n", q->tokens[i].token);
	}

	bamboo_clean(handle);

	std::vector<bamboo::Token> x;
	bamboo::Parser p("");
	p.parse(x, argv[1]);

	for (i = 0; i < x.size(); i++) {
		printf("%s\n", x[i].token);
	}

}
