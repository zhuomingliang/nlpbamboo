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

#include <sys/time.h>

#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <stdio.h>

#include "parser_factory.hxx"
#include "custom_parser.hxx"

const char g_default_parser[] = "crf_seg";
const char g_default_config[] = "";
const char g_default_file[] = "-";
const char *g_config = g_default_config, *g_file = g_default_file, *g_parser = g_default_parser;
bool g_verbose = false;

std::vector<std::string> g_override;

static void _help_message()
{
	std::cout << "Usage: bamboo_cli [OPTIONS] file\n"
				 "OPTIONS:\n"
				 "        -c|--config           configuration\n"
				 "        -h|--help             help message\n"
				 "        -p|--parser           parser, default: crf_seg\n"
				 "        -v|--verbose          verbose\n"
				 "\n"
				 "Report bugs to detrox@gmail.com\n"
			  << std::endl;
}

static int _do()
{
	char *s, *t;
	ssize_t length;
	size_t n;
	FILE *fp = NULL;
	struct timeval tv[2];
	struct timezone tz;
	unsigned long consume = 0;
	std::vector<bamboo::Token *> vec;
	std::vector<bamboo::Token *>::iterator it;

	try {
		bamboo::Parser *parser;
		bamboo::ParserFactory *factory;

		factory = bamboo::ParserFactory::get_instance();
		parser = factory->create(g_parser, g_config, g_verbose);
		if (parser == NULL)
			throw std::runtime_error(std::string("parser can not be found: ") + g_parser);
		std::cerr << "parsing '" << g_file << "'..." << std::endl;
		if (strcmp(g_file, "-") == 0) {
			fp = stdin;
		} else {
			fp = fopen(g_file, "r");
			if (fp == NULL) {
				std::cerr << "can not read file " << g_file << std::endl;
				return 1;
			}
		}
		s = NULL;
		while ((length = getline(&s, &n, fp)) > 0) {
			vec.clear();
			if (length >= 1 && s[length - 1] == '\n') s[length - 1] = '\0';
			if (length >= 2 && s[length - 2] == '\r') s[length - 2] = '\0';
			gettimeofday(&tv[0], &tz);
			for (t = strtok(s, "\n"); t; t = strtok(NULL, "\n")) {
				parser->setopt(BAMBOO_OPTION_TEXT, t);
				parser->parse(vec);
			}
			gettimeofday(&tv[1], &tz);
			consume += (tv[1].tv_sec - tv[0].tv_sec) * 1000000 + (tv[1].tv_usec - tv[0].tv_usec);

			for (it = vec.begin(); it < vec.end(); ++it) {
				if (it != vec.begin()) {
					std::cout << " ";
				}
				unsigned short pos = (*it)->get_pos();
				std::cout << (*it)->get_orig_token();
				if (pos) {
					char ch;
					std::cout << "/";
					ch = pos/256;
					if (ch)
						std::cout << ch;
					ch = pos%256;
					if (ch)
						std::cout << ch;
				}
				delete *it;
			}
			std::cout << std::endl;
		}
		free(s);
		delete parser;
	} catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	std::cerr << "consumed time: " << static_cast<double>(consume / 1000)<< " ms" << std::endl;
	return 0;
}

int main(int argc, char *argv[])
{
	int c;
	
	while (true) {
		static struct option long_options[] =
		{
			{"help", no_argument, 0, 'h'},
			{"config", required_argument, 0, 'c'},
			{"parser", required_argument, 0, 'p'},
			{"verbose", required_argument, 0, 'v'},
			{"set", required_argument, 0, 's'},
			{0, 0, 0, 0}
		};
		int option_index;
		
		c = getopt_long(argc, argv, "c:hp:s:v", long_options, &option_index);
		if (c == -1) break;

		switch(c) {
			case 'h':
				_help_message();
				return 0;
			case 'c':
				g_config = optarg;
				break;
			case 'p':
				g_parser = optarg;
				break;
			case 's':
				g_override.push_back(optarg);
				break;
			case 'v':
				g_verbose = true;
				std::cerr << "verbose on" << std::endl;
				break;
		}
	}

	if (optind < argc)
		g_file = argv[optind];

	return _do();
}

