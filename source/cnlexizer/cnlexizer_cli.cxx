#include <sys/time.h>

#include <getopt.h>
#include <iostream>
#include <stdexcept>

#include "cnlexizer.hxx"

static void _help_message()
{
	std::cout << "Usage: cnlexizer_cli [OPTIONS] file\n"
				 "OPTIONS:\n"
				 "        -h|--help             help message\n"
				 "        -c|--config           configuration\n"
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
	struct timeval tv[2];
	struct timezone tz;
	size_t consume = 0;

	try {
		CNLexizer clx(cfg);
		std::cerr << "parsing '" << file << "'..." << std::endl;
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
			if (s[length - 1] == '\n') s[length - 1] = '\0';
			if (s[length - 2] == '\r') s[length - 2] = '\0';
			if (m < n) {
				m = (n << 2) + 1;
				t = (char *)realloc(t, m);
				if (t == NULL) 
					throw std::bad_alloc();
			}
			gettimeofday(&tv[0], &tz);
			clx.process(t, s);
			gettimeofday(&tv[1], &tz);
			consume += (tv[1].tv_sec - tv[0].tv_sec) * 1000000 + (tv[1].tv_usec - tv[0].tv_usec);
			std::cout << t << std::endl;
		}
		free(s);
		free(t);
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
	const char default_config[] = "";
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
