#include <iostream>
#include <vector>
#include <cstdlib>
#include <unistd.h>

#include "bamboo.hxx"

using namespace bamboo;

#define USAGE "regress_ner -t nr|ns|nt"

int main(int argc, char * argv[])
{
	const char * ner_type = NULL;
	int opt;
	while( (opt=getopt(argc, argv, "t:")) != -1) {
		switch(opt) {
		case 't':
			ner_type = optarg;
			break;
		case '?':
			printf("%s\n", USAGE);
			exit(1);
		}
	}

	if(!ner_type) {
		printf("%s\n", USAGE);
		exit(1);
	}

	std::vector<Token *> v;
	std::vector<Token *>::iterator it;

	char parser_name[16];
	sprintf(parser_name, "crf_ner_%s", ner_type);

	ParserFactory *factory = ParserFactory::get_instance();
	Parser *p = factory->create(parser_name);

	const char * text = "中共中央在北京召开会议，温家宝同志主持了会议。";
	p->setopt(BAMBOO_OPTION_TEXT, const_cast<char *>(text));
	p->parse(v);

	for (it = v.begin(); it != v.end(); it++) {
		std::cout << (*it)->get_orig_token() << std::endl;
		delete *it;
	}

	delete p;

	return 0;
}
