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
