#include <cassert>
#include <iostream>

#include "lexicon_factory.hxx"
#include "single_combine_processor.hxx"
#include "utf8.hxx"

SingleCombineProcessor::SingleCombineProcessor(IConfig *config)
	:_max_token_length(16)
{
	const char *s;

	config->get_value("lexicon_combine", s);
	_lexicon_combine = LexiconFactory::load(s);
	config->get_value("lexicon_number_trailing", s);
	_lexicon_number_trailing = LexiconFactory::load(s);
	config->get_value("max_token_length", _max_token_length);

//	_combine.base = new char[_max_token_length << 2 + 1]; /* x4 for unicode */
}

SingleCombineProcessor::~SingleCombineProcessor()
{
//	delete []_combine.base;
	delete _lexicon_combine;
	delete _lexicon_number_trailing;
}

void SingleCombineProcessor::process(std::vector<LexToken *> &in, std::vector<LexToken *> &out)
{
	size_t i, j, size, num, length, match;
	const char *s;

	size = in.size();
	for (i = 0; i < size; i++) {
		match = 0;
		if (!in[i]) continue;
		length = in[i]->get_length();
		if (i + 1 < size && in[i + 1]
				  && in[i]->get_attr() == LexToken::attr_number 
				  && _lexicon_number_trailing->search(in[i + 1]->get_token()))
		{
			_make_combine(in, i, 3);
			match = 3;
		}
		if (!match && length == 1 && i > 0 && i + 1 < size && in[i - 1] && in[i + 1]) {
			_make_combine(in, i, 7);
			if (_lexicon_combine->search(s) > 0) match = 7; 
		}
		if (!match && length == 1 && i > 0 && in[i - 1]) {
			_make_combine(in, i, 6);
			if (_lexicon_combine->search(s) > 0) match = 6;
		}
		if (!match && length == 1 && i + 1 < size && in[i + 1]) {
			_make_combine(in, i, 3);
			if (_lexicon_combine->search(s) > 0) match = 3;
		}

		s = _combine.c_str();
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
			out.push_back(new LexToken(s, LexToken::attr_cword));
		} else {
			out.push_back(in[i]);
		}
	}
}
