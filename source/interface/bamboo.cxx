#include <iostream>
#include "bamboo.hxx"

using namespace bamboo;

void *bamboo_init(const char *cfg)
{
	ParserFactory *factory;
	try {
		factory = ParserFactory::get_instance();
		return factory->create("crf_seg");
	} catch(std::exception &e) {
		std::cerr << __func__ << ": " << e.what();
		return NULL;
	}
}
