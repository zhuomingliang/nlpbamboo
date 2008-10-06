/*
 * Copyright (c) 2008, detrox@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY detrox@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL detrox@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

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

