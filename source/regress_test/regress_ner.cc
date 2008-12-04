#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <unistd.h>

#include "bamboo.hxx"

using namespace bamboo;

#define USAGE "regress_ner -t nr|ns|nt [-f input_file]"

int main(int argc, char * argv[])
{
	const char * ner_type = NULL, * input_file = NULL;
	int opt;
	while( (opt=getopt(argc, argv, "t:f:")) != -1) {
		switch(opt) {
		case 't':
			ner_type = optarg;
			break;
		case 'f':
			input_file = optarg;
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

	std::istream * is;
	std::ifstream fin;

	if(!input_file) {
		is = & std::cin;
	} else {
		fin.open(input_file);
		is = & fin;
	}

	std::string text = "";

	while(!is->eof()) {
		std::string line;
		getline(*is, line);
		text += line + "\n";
	}

	if(input_file) {
		fin.close();
	}

	std::vector<Token *> v;
	std::vector<Token *>::iterator it;

	char parser_name[16];
	sprintf(parser_name, "crf_ner_%s", ner_type);

	ParserFactory *factory = ParserFactory::get_instance();
	Parser *p = factory->create(parser_name);

	p->setopt(BAMBOO_OPTION_TEXT, const_cast<char *>(text.c_str()));
	p->parse(v);

	for (it = v.begin(); it != v.end(); it++) {
		std::cout << (*it)->get_orig_token() << std::endl;
		delete *it;
	}

	delete p;

	return 0;
}
