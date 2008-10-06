#include <cassert>
#include <iostream>

#include "lexicon_factory.hxx"
#include "single_combine_processor.hxx"
#include "utf8.hxx"

PROCESSOR_MAGIC
PROCESSOR_MODULE(SingleCombineProcessor)

SingleCombineProcessor::SingleCombineProcessor(IConfig *config)
{
	const char *s;

	config->get_value("lexicon_combine", s);
	_lexicon_combine = LexiconFactory::load(s);
	config->get_value("lexicon_number_trailing", s);
	_lexicon_number_trailing = LexiconFactory::load(s);
}

SingleCombineProcessor::~SingleCombineProcessor()
{
	delete _lexicon_combine;
	delete _lexicon_number_trailing;
}

void SingleCombineProcessor::process(std::vector<LexToken *> &in, std::vector<LexToken *> &out)
{
	size_t i, j, size, num, length, match;
	int attr;

	size = in.size();
	for (i = 0; i < size; i++) {
		if (in[i] == NULL) continue;
		match = 0;
		length = in[i]->get_length();
		attr = in[i]->get_attr();
		
		if (i + 1 < size && in[i + 1]
				  && in[i]->get_attr() == LexToken::attr_number 
				  && _lexicon_number_trailing->search(in[i + 1]->get_token()))
		{
			_make_combine(in, i, 3);
			match = 3;
		}
		
		if (!match && length == 1 && i > 0 && i + 1 < size && in[i - 1] && in[i + 1]) {
			_make_combine(in, i, 7);
			if (_lexicon_combine->search(_combine.c_str()) > 0) match = 7; 
		}
		if (!match && length == 1 && i > 0 && in[i - 1]) {
			_make_combine(in, i, 6);
			if (_lexicon_combine->search(_combine.c_str()) > 0) match = 6;
		}
		if (!match && length == 1 && i + 1 < size && in[i + 1]) {
			_make_combine(in, i, 3);
			if (_lexicon_combine->search(_combine.c_str()) > 0) match = 3;
		}
		if (!match && length == 1 && i > 0 && in[i - 1]
				   && in[i - 1]->get_length() == 1
				   && strcmp(in[i]->get_token(), in[i - 1]->get_token()) == 0) {
			_make_combine(in, i, 6);
			match = 6;
		}
		if (match) {
			if (match & 4) {
				out.pop_back();
				delete in[i - 1];
				in[i - 1] = NULL;
			} 
			if (match & 2) {
				delete in[i];
				in[i] = NULL;
			} 
			if (match & 1) {
				delete in[i + 1];
				in[i + 1] = NULL;
			}
			out.push_back(new LexToken(_combine.c_str(), attr));
		} else {
			out.push_back(in[i]);
		}
	}
}
