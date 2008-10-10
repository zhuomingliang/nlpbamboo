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

#include <getopt.h>
#include <iostream>
#include <stdexcept>

#include "lexicon_factory.hxx"


static void _info(const char *index)
{
	bamboo::ILexicon *dc;

	assert(index);
	dc = bamboo::LexiconFactory::load(index);

	std::cout << " max = " << dc->max_value() 
			  << " min = " << dc->min_value()
			  << " sum = " << dc->sum_value()
			  << std::endl;
}

static void _query(const char *index, const char *query)
{
	bamboo::ILexicon *dc;

	assert(query); assert(index);
	dc = bamboo::LexiconFactory::load(index);

	std::cout << query << " = " << dc->search(query) << std::endl;
}

static void _dump(const char *index, const char *target)
{
	bamboo::ILexicon *dc;

	assert(target); assert(index);
	dc = bamboo::LexiconFactory::load(index);
	if (dc == NULL) throw std::runtime_error("can not load lexicon");
	dc->write_to_text(target);
}

static void _build(const char *source, const char *index, const char *type, bool verbose)
{
	bamboo::ILexicon *dc;

	assert(source); assert(index);
	dc = bamboo::LexiconFactory::create(type);
	if (dc == NULL) throw std::runtime_error("can not create lexicon");
	dc->read_from_text(source, verbose);
	dc->save(index);
}

static void _help_message()
{
	std::cout << "Usage: lexicon [OPTIONS]\n"
				 "OPTIONS:\n"
				 "        -h|--help             help message\n"
				 "        -i|--index            index file\n"
				 "        -s|--source           source file\n"
				 "        -b|--build            build index, needs -i and -s\n"
				 "        -d|--dump             dump index, needs -i\n"
				 "        -q|--query QUERY      query index, needs -i\n"
				 "        -t|--type TYPE        index type, default = DATrie\n"
				 "        -n|--info             index information, needs -i\n"
				 "        -v|--verbose          verbose\n"
				 "\n"
				 "Report bugs to jianing.yang@alibaba-inc.com\n"
			  << std::endl;
}

int main(int argc, char *argv[])
{
	int c;
	const char default_type[] = "datrie";
	const char *index = NULL, *source = NULL, *query = NULL, *type = default_type, *dump = NULL;
	bool verbose = false;
	enum action_t {
		ACTION_NO = 0,
		ACTION_BUILD = 1,
		ACTION_DUMP = 2,
		ACTION_QUERY = 3,
		ACTION_INFO = 4
	} action = ACTION_NO;
	
	while (true) {
		static struct option long_options[] =
		{
			{"help", no_argument, 0, 'h'},
			{"build", no_argument, 0, 'b'},
			{"dump", required_argument, 0, 'd'},
			{"query", required_argument, 0, 'q'},
			{"index", required_argument, 0, 'i'},
			{"source", required_argument, 0, 's'},
			{"type", required_argument, 0, 't'},
			{"info", no_argument, 0, 'n'},
			{"verbose", no_argument, 0, 'v'},
			{0, 0, 0, 0}
		};
		int option_index;
		
		c = getopt_long(argc, argv, "hbd:q:i:s:t:nv", long_options, &option_index);
		if (c == -1) break;

		switch(c) {
			case 'h':
				_help_message();
				return 0;
			case 'b':
				action = ACTION_BUILD;
				break;
			case 'd':
				action = ACTION_DUMP;
				dump = optarg;
				break;
			case 'n':
				action = ACTION_INFO;
				break;
			case 'q':
				action = ACTION_QUERY;
				query = optarg;
				break;
			case 'i':
				index = optarg;
				break;
			case 's':
				source = optarg;
				break;
			case 't':
				type = optarg;
				break;
			case 'v':
				verbose = true;
				break;
		}

	}

	if (action == ACTION_BUILD && index && source && type) {
		_build(source, index, type, verbose);
	} else if (action == ACTION_QUERY && index && query) {
		_query(index, query);
	} else if (action == ACTION_DUMP && index && dump) {
		_dump(index, dump);
	} else if (action == ACTION_INFO && index) {
		_info(index);
	} else {
		_help_message();
	}

	return 0;
}

