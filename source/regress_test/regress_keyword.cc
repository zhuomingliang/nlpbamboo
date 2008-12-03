#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <unistd.h>

#include "bamboo.hxx"

using namespace bamboo;

#define USAGE "regress_ner -f input_file"

int main(int argc, char * argv[])
{
	const char * input_file = NULL;
	std::istream * is;
	std::ifstream fin;

	int opt;
	while( (opt=getopt(argc, argv, "f:")) != -1) {
		switch(opt) {
		case 'f':
			input_file = optarg;
			break;
		case '?':
			printf("%s\n", USAGE);
			exit(1);
		}
	}

	if(!input_file) {
		is = & std::cin;
	} else {
		fin.open(input_file);
		is = & fin;
	}

	std::string title = "", text = "";
	size_t i = 0;

	while(!is->eof()) {
		std::string line;
		getline(*is, line);
		if(i == 0) {
			title = line;
		}
		else {
			text += line + "\n";
		}
		++i;
	}

	if(input_file) {
		fin.close();
	}

	std::vector<Token *> v;
	std::vector<Token *>::iterator it;

	ParserFactory *factory = ParserFactory::get_instance();
	Parser *p = factory->create("keyword");

	p->setopt(BAMBOO_OPTION_TITLE, const_cast<char *>(title.c_str()));
	p->setopt(BAMBOO_OPTION_TEXT, const_cast<char *>(text.c_str()));
	p->parse(v);

	for (it = v.begin(); it != v.end(); it++) {
		std::cout << (*it)->get_orig_token() << std::endl;
		delete *it;
	}

	delete p;

	return 0;
}
