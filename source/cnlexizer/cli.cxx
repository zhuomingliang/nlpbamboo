#include <getopt.h>
#include <iostream>
#include <stdexcept>

#include "cnlexizer.hxx"

static void _help_message()
{
	std::cout << "Usage: cnlexizer [OPTIONS] file\n"
				 "OPTIONS:\n"
				 "        -h|--help             help message\n"
				 "        -c|--config           config file, default=/etc/sgmt_cn.cfg\n"
				 "        -m|--method           method use to do segmentation availables are ugm,bgm,mfw,mbw\n"
				 "                              default to unigram\n"
				 "\n"
				 "Report bugs to jianing.yang@alibaba-inc.com\n"
			  << std::endl;
}

static int _do(const char *cfg, const char *method, const char *file)
{
	char s[4096], t[8192];
	FILE *fp = NULL;
	size_t (CNLexizer::*f)(char *, const char*) = NULL;
	try {
		std::cerr << "Loading configuration " << cfg << std::endl;
		CNLexizer cns(cfg);
		std::cerr << "Opening text " << file << std::endl;
		if (strcmp(file, "-") == 0) {
			fp = stdin;
		} else {
			fp = fopen(file, "r");
			if (fp == NULL) {
				std::cerr << "can not read file " << file << std::endl;
				return 1;
			}
		}
		if (strcmp(method, "ugm") == 0) {
			f = &CNLexizer::unigram;
		} else if (strcmp(method, "bgm") == 0) {
			f = &CNLexizer::bigram;
		} else if (strcmp(method, "mfw") == 0) {
			f = &CNLexizer::maxforward;
		} else if (strcmp(method, "mbw") == 0) {
			f = &CNLexizer::maxbackward;
		}

		std::cerr << "Using method " << method << std::endl;
		while (EOF != fscanf(fp, "%4096s", s)) {
			if (f) (cns.*f)(t, s);
			std::cout << t << "\n";	
		}
	} catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int c;
	const char default_config[] = "/etc/sgmt_cn.cfg";
	const char default_method[] = "ugm";
	const char default_file[] = "-";
	const char *method = default_method, *config = default_config, *file = default_file;
	
	while (true) {
		static struct option long_options[] =
		{
			{"help", no_argument, 0, 'h'},
			{"config", required_argument, 0, 'c'},
			{"method", required_argument, 0, 'm'},
			{0, 0, 0, 0}
		};
		int option_index;
		
		c = getopt_long(argc, argv, "c:hm:", long_options, &option_index);
		if (c == -1) break;

		switch(c) {
			case 'h':
				_help_message();
				return 0;
			case 'c':
				config = optarg;
				break;
			case 'm':
				method = optarg;
				break;
		}
	}

	if (optind < argc)
		file = argv[optind];

	return _do(config, method, file);
}
