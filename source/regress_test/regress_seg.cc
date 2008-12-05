#include <iostream>
#include <vector>
#include <cstdlib>

#include "bamboo.hxx"

using namespace bamboo;

int main()
{
	std::vector<Token *> v;
	std::vector<Token *>::iterator it;

	ParserFactory *factory = ParserFactory::get_instance();
	Parser *p = factory->create("crf_seg");

	const char * text = "我爱北京天安门";
	p->setopt(BAMBOO_OPTION_TEXT, const_cast<char *>(text));
	p->parse(v);

	for (it = v.begin(); it != v.end(); it++) {
		std::cout << (*it)->get_orig_token() << std::endl;
		delete *it;
	}

	delete p;

	void *handle = bamboo_init("crf_seg",NULL);
	char *t = NULL;
	int c = 20;
	while (c--) {
		bamboo_setopt(handle, BAMBOO_OPTION_TEXT, const_cast<char *>(text));
		printf("==> %s\n", (char *)bamboo_getopt(handle, BAMBOO_OPTION_TEXT));
		t = bamboo_parse(handle);
		printf("==> %s\n", t);
		free(t);
	}
	bamboo_clean(handle);


	return 0;
}
