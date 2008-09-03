#include <getopt.h>
#include <iostream>
#include <cstring>
#include <stdexcept>

#include "config_factory.hxx"

static void _help_message()
{
	std::cout << "Usage: cf_tool [OPTIONS]\n"
				 "OPTIONS:\n"
				 "        -h|--help             help message\n"
				 "        -c|--cfg              config file\n"
				 "        -k|--key              key name\n"
				 "        -t|--type TYPE        key type, default = string, options = {string, int}\n"
				 "\n"
				 "Report bugs to jianing.yang@alibaba-inc.com\n"
			  << std::endl;
}

void _query(const char *cf, const char *key, const char *type) 
{
	IConfig *icf;
	int int_val;
	const char *str_val;
	icf = ConfigFactory::create("lua_config", cf);
	if (strcmp(type, "int") == 0) {
		icf->get_value(key, int_val);
		std::cout << key << " = " << int_val << std::endl;
	} else if (strcmp(type, "string") == 0) {
		icf->get_value(key, str_val);
		std::cout << key << " = " << str_val << std::endl;
	} else {
		std::cerr << "Unknow key type: " << type << std::endl;
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
			{"info", no_argument, 0, 'n'},
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
