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

	void *handle = bamboo_init_ex("crf_seg",NULL);
	char *t = NULL;
	bamboo_parse(handle, &t, "我爱北京天安门");
	printf("==> %s\n", t);
	free(t);
	bamboo_clean(handle);


	return 0;
}
