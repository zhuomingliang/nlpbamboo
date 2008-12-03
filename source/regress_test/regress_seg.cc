#include <iostream>
#include <vector>

#include "bamboo.hxx"

using namespace bamboo;

int main()
{
	std::vector<Token *> v;
	std::vector<Token *>::iterator it;

	ParserFactory *factory = ParserFactory::get_instance();
	Parser *p = factory->create("crf_seg");
	p->parse(v, "我爱北京天安门");

	for (it = v.begin(); it != v.end(); it++) {
		std::cout << (*it)->get_orig_token() << std::endl;
		delete *it;
	}

	delete p;

	return 0;
}
