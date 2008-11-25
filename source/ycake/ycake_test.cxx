#include "ycake.hxx"
#include "unistd.h"
#include <iostream>
#include <fstream>
#include <map>

using namespace bamboo::ycake;

#define USAGE "ycake_word_train [-c config] -f test_file"

int main(int argc, char *argv[]) {

	const char * ycake_cfg = NULL, * test_file = NULL;

	int opt;
	while( (opt=getopt(argc, argv, "c:f:")) != -1) {
		switch(opt) {
		case 'c':
			ycake_cfg = optarg;
			break;
		case 'f':
			test_file = optarg;
			break;
		case '?':
			std::cerr << USAGE << std::endl;
			exit(1);
		}
	}

        KeywordExtractor p(ycake_cfg);
	std::string file_content, tmp_str;

	std::istream * file_stream;
	std::ifstream fin;

	if(test_file) {
		fin.open(test_file);
		file_stream = &fin;
	} else {
		file_stream = & std::cin;
	}

	while(!file_stream->eof()) {
		std::getline(*file_stream, tmp_str);
		file_content += tmp_str;
	}

	if(test_file)
		fin.close();

        std::vector<std::string> res;
	p.get_keyword(file_content.c_str(), res);

        for(size_t i=0; i<res.size(); ++i) {
		std::cout << res[i] << "\t";
        }
	std::cout<<std::endl;
        return 0;
}

