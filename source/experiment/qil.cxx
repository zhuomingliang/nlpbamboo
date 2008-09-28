#include <string>
#include <fstream>
#include <iostream>
#include <getopt.h>

#include "datrie.hxx"

class QueryInLog {
protected:
	static struct option _long_opts[];
	static char _short_opts[];
	std::string _query_log;
	std::string _query_file;
	std::string _index;
	bool _verbose;

	void _help()
	{
		std::cerr << "Usage: qil [OPTIONS] INDEX\n"
				  << "Print number of appearance of given queries in a query log\n"
				  << "\nOPTIONS: \n"
				  << "        -a|--analyze QUERYFILE      Analyze by given QUERYFILE\n"
				  << "        -b|--build QUERYLOG         Build a trie index of QUERYLOG, save in INDEX\n"
				  << "        -h|--help                   Help message\n"
				  << "\nEXAMPLES: \n"
				  << "  $ qil -b 2008-09-15.log 2008-09-15.idx\n"
				  << "  $ qil -a queries 2008-09-15.idx\n"
				  << std::endl;
	}

	void _build()
	{
		std::fstream fs;
		std::string s;
		int val;
		DATrie trie;

		if (_verbose)
			std::cerr << "Building " << _index << " from " << _query_log << std::endl;

		fs.open(_query_log.c_str(), std::fstream::in);
		while (!fs.eof()) {
			std::getline(fs, s);
			if (!s.empty()) {
				if ((val = trie.search(s.c_str())) > 0) {
					trie.insert(s.c_str(), val + 1);
				} else {
					trie.insert(s.c_str(), 1);
				}
			}
		}
		fs.close();
		trie.save(_index.c_str());
	}

	void _analyze()
	{
		std::fstream fs;
		std::string s;
		int val;
		DATrie trie(_index.c_str());

		if (_verbose)
			std::cerr << "Analyze " << _index << " by " << _query_file << std::endl;

		fs.open(_query_file.c_str(), std::fstream::in);
		while (!fs.eof()) {
			std::getline(fs, s);
			if (!s.empty()) {
				if ((val = trie.search(s.c_str())) > 0) {
					std::cout << s << "\t" << val << std::endl;
				}
			}
		}
		fs.close();
	}

public:
	QueryInLog(int argc, char *argv[])
		:_verbose(false)
	{
		int opt_index, opt;
		for (;;) {
			if ((opt = getopt_long(argc, argv, _short_opts, _long_opts, &opt_index)) < 0) break;
			switch(opt) {
				case 'a':
					_query_file = optarg;
					break;
				case 'b':
					_query_log = optarg;
					break;
				case 'h':
					_help();
					break;
				case 'v':
					_verbose = true;
					break;
			}
		}
		if (!_query_log.empty() && !_query_file.empty()) {
			std::cerr << "ambiguous option -a and -b" << std::endl;
		} else if (optind < argc && !_query_file.empty()) {
			_index = argv[optind];
			_analyze();
		} else if (optind < argc && !_query_log.empty()) {
			_index = argv[optind];
			_build();
		} else {
			_help();
		}
	}
};

struct option QueryInLog::_long_opts[] =
{
	{"analyze", required_argument, 0, 'a'},
	{"build", required_argument, 0, 'b'},
	{"verbose", no_argument, 0, 'v'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

char QueryInLog::_short_opts[] = "a:b:hv";

int main(int argc, char *argv[])
{
	QueryInLog qil(argc, argv);

	return 0;
}

