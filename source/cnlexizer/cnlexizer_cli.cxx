#include <getopt.h>
#include <iostream>
#include <stdexcept>

#include "cnlexizer.hxx"

static void _help_message()
{
	std::cout << "Usage: cnlexizer_cli [OPTIONS] file\n"
				 "OPTIONS:\n"
				 "        -h|--help             help message\n"
				 "        -c|--config           config file, default=./etc/cnlexizer.cfg\n"
				 "\n"
				 "Report bugs to detrox@gmail.com\n"
			  << std::endl;
}

static int _do(const char *cfg, const char *file)
{
	char *s, *t;
	size_t n, m;
	ssize_t length;
	FILE *fp = NULL;

	try {
		std::cerr << "Loading configuration " << cfg << std::endl;
		CNLexizer clx(cfg);
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
		t = s = NULL;
		n = 0; m = 0;
		while ((length = getline(&s, &n, fp)) > 0) {
			s[length - 1] = '\0';
			if (m < n) {
				m = (n << 2) + 1;
				t = (char *)realloc(t, m);
				if (t == NULL) 
					throw std::bad_alloc();
			}
			clx.process(t, s);
			std::cout << t << "\n";	
		}
		free(s);
		free(t);
	} catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int c;
	const char default_config[] = "etc/cnlexizer.cfg";
	const char default_file[] = "-";
	const char *config = default_config, *file = default_file;
	
	while (true) {
		static struct option long_options[] =
		{
			{"help", no_argument, 0, 'h'},
			{"config", required_argument, 0, 'c'},
			{0, 0, 0, 0}
		};
		int option_index;
		
		c = getopt_long(argc, argv, "c:h", long_options, &option_index);
		if (c == -1) break;

		switch(c) {
			case 'h':
				_help_message();
				return 0;
			case 'c':
				config = optarg;
				break;
		}
	}

	if (optind < argc)
		file = argv[optind];

	return _do(config, file);
}
