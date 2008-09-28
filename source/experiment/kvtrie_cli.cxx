#include <fstream>
#include <iostream>
#include <getopt.h>
#include <string>

#include "kvtrie.hxx"

static char _short_opts[] = "b:s:h";
static struct option _long_opts[] =
{
	{"build", required_argument, 0, 'b'},
	{"search", required_argument, 0, 's'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static void _help()
{
	std::cerr << "Usage: kvtrie_cli [OPTIONS] INDEX DATA\n"
			  << "Key-Value Trie Command Line Tool\n"
			  << "\nOPTIONS: \n"
			  << "        -b|--build SOURCE           Build a trie index from SOURCE\n"
			  << "        -s|--search FILE            Search from FILE\n"
			  << "        -h|--help                   Help message\n"
			  << std::endl;
}

void _search(const char *index, const char *data, const char *search)
{
	KVTrie kvt(index, data);
	std::ifstream ifs(search);
	std::string s;
	const char *v;

	while(!ifs.eof()) {
		std::getline(ifs, s);
		if (s.empty()) continue;
		v = kvt[s.c_str()];	
		if (v)
			std::cout << s << " " << v << std::endl;
	}
}

int main(int argc, char *argv[])
{
	int opt_index, opt;
	enum {
		ACT_BUILD = 1,
		ACT_SEARCH,
		ACT_HELP,
		ACT_END
	} act = ACT_HELP;

	const char *source, *search;

	for (;;) {
		if ((opt = getopt_long(argc, argv, _short_opts, _long_opts, &opt_index)) < 0) break;
		switch(opt) {
			case 's':
				act = ACT_SEARCH;
				search = optarg;
				break;
			case 'b':
				act = ACT_BUILD;
				source = optarg;
				break;
			case 'h':
				act = ACT_HELP;
				break;
		}
	}
	if (optind + 2 < argc) {
		std::cerr << "missing arugment" << std::endl;
		act = ACT_HELP;
	}
	switch(act) {
		case ACT_BUILD:
			KVTrie::build_from_text(argv[optind], argv[optind + 1], source, false);
			break;
		case ACT_SEARCH:
			_search(argv[optind], argv[optind + 1], search);
			break;
		case ACT_HELP:
			_help();
			break;
	}
}
