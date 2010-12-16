#include <getopt.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "iconfig.hxx"
#include "simple_config.hxx"
#include "token_impl.hxx"
#include "processor_factory.hxx"
#include "prepare_processor.hxx"

using namespace std;
using namespace bamboo;

void print_usage() {
    cout << "crf2_tool -i input_file -o output_file" << endl;
}

int main(int argc, char **argv) {
    int opt;
    char *input_filename = NULL;
    char *output_filename = NULL;
    while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
        switch (opt) {
        case 'i':
            input_filename = optarg;
            break;
        case 'o':
            output_filename = optarg;
            break;
        case 'h':
            print_usage();
            return EXIT_SUCCESS;
        default:
            cerr << "unknown option" << endl;
            print_usage();
            return EXIT_FAILURE;
        }
    }
    if (input_filename == NULL || output_filename == NULL) {
        if (input_filename == NULL) {
            cerr << "no input_filename!" << endl;
        }
        if (output_filename == NULL) {
            cerr << "no ouput_filename!" << endl;
        }
        print_usage();
        return EXIT_FAILURE;
    }

    SimpleConfig config;
    config["concat_hyphen"] = "1";
    config["prepare_characterize"] = "1"; 
    ProcessorFactory *processor_factory = ProcessorFactory::get_instance();
    processor_factory->set_config(&config);
   
    Processor *prepare_processor = processor_factory->create("prepare");
    vector<TokenImpl *> in;
    vector<TokenImpl *> out;
    ifstream ifs(input_filename);
    if (!ifs) throw runtime_error("input file open error: " + string(input_filename));
    ofstream ofs(output_filename);
    if (!ofs) throw runtime_error("output file open error: " + string(output_filename));
    string line, segment, last_tag, tag;
    while (getline(ifs, line)) {
        if (line.size() == 0)
            continue;
        istringstream iss(line);
		bool has_output = false;
        while (iss >> segment) {
            in.clear();
            in.push_back(new TokenImpl(segment.c_str()));
            out.clear();
            prepare_processor->process(in, out);
            if (out.size() > 0) {
                for (vector<TokenImpl *>::size_type pos = 0; pos < out.size(); ++pos) {
                    if (pos == 0 && pos + 1 == out.size()) {
                        tag = "S";
                    } else if (pos == 0) {
                        tag = "B";
                    } else if (pos + 1 == out.size()) {
                        tag = "E";
                    } else if (last_tag == "B") {
                    	tag = "B1";
                    } else if (last_tag == "B1") {
                    	tag = "B2";
                    } else {
                    	tag = "M";
                    }
                   	last_tag = tag; 
                    const char *word = out[pos]->get_token();
                    const char *type = PrepareProcessor::get_crf2_tag(out[pos]);
                    ofs << word << " " << type << " " << tag << endl;
					if (!has_output) {
						has_output = true;
					}
                    delete out[pos];
                }
            }
        }
		if (has_output) {
	    	ofs << endl;
		}
    }
    ofs.close();
    ifs.close();
    delete prepare_processor;
    delete processor_factory;
    return EXIT_SUCCESS;
}
