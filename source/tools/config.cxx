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
#include <cstring>
#include <stdexcept>

#include "config_factory.hxx"

static void _help_message()
{
	std::cout << "Usage: config_parser [OPTIONS]\n"
				 "OPTIONS:\n"
				 "        -h|--help             help message\n"
				 "        -c|--cfg              config file\n"
				 "        -k|--key              key name\n"
				 "        -t|--type TYPE        key type, default = string, options = {string, int, long, double}\n"
				 "\n"
				 "Report bugs to jianing.yang@alibaba-inc.com\n"
			  << std::endl;
}

void _query(const char *cf, const char *key, const char *type) 
{
	bamboo::IConfig *icf;
	int int_val;
	const char *str_val;
	long long_val;
	double double_val;

	icf = bamboo::ConfigFactory::create(cf);
	if (strcmp(type, "int") == 0) {
		icf->get_value(key, int_val);
		std::cout << key << " = " << int_val << std::endl;
	} else if (strcmp(type, "string") == 0) {
		icf->get_value(key, str_val);
		std::cout << key << " = " << str_val << std::endl;
	} else if (strcmp(type, "long") == 0) {
		icf->get_value(key, long_val);
		std::cout << key << " = " << long_val << std::endl;
	} else if (strcmp(type, "double") == 0) {
		icf->get_value(key, double_val);
		std::cout << key << " = " << double_val << std::endl;
	} else {
		std::cerr << "unsupported type: " << type << std::endl;
		_help_message();
	}
}

int main(int argc, char *argv[])
{
	int c;
	const char default_type[] = "string";
	const char *cf = NULL, *key = NULL, *type = default_type; 
	
	while (true) {
		static struct option long_options[] =
		{
			{"help", no_argument, 0, 'h'},
			{"config", required_argument, 0, 'c'},
			{"key", required_argument, 0, 'k'},
			{"type", required_argument, 0, 't'},
			{0, 0, 0, 0}
		};
		int option_index;
		
		c = getopt_long(argc, argv, "hc:k:t:", long_options, &option_index);
		if (c == -1) break;

		switch(c) {
			case 'h':
				_help_message();
				return 0;
			case 'c':
				cf = optarg;
				break;
			case 'k':
				key = optarg;
				break;
			case 't':
				type = optarg;
				break;
		}

	}

	if (cf && key && type) {
		_query(cf, key, type);
	} else {
		_help_message();
	}

	return 0;
}
