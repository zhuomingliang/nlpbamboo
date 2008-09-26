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

	_combine.base = new char[_max_token_length << 2 + 1]; /* x4 for unicode */
}

SingleCombineProcessor::~SingleCombineProcessor()
{
	delete []_combine.base;
	delete _lexicon_combine;
	delete _lexicon_number_trailing;
}

void SingleCombineProcessor::process(std::vector<LexToken *> &in, std::vector<LexToken *> &out)
{
	size_t i, j, size, num;
	int with;
	bool check;

	size = in.size();
	for (i = 0; i < size; i++) {
		check = false;
		with = 0;
		if (in[i]->get_length() < 3) {
			if (i > 0 && in[i - 1]
					  && in[i - 1]->get_attr() == LexToken::attr_number 
					  && _lexicon_number_trailing->search(in[i]->get_token()))
			{
				with = 6;
				check = false;
			} else if (i > 0 && in[i - 1] && in[i - 1]->get_length() == 1) {
				with = 6;
				check = true;
			} else if (i > 0 && i + 1 < size && in[i - 1] && in[i + 1] 
					   && in[i - 1]->get_length() == 1 
					   && in[i + 1]->get_length() == 1) {
				with = 7;
				check = true;
			}
		}

		_combine.top = _combine.base;
		if (with & 4) {
			strcpy(_combine.top, in[i - 1]->get_orig_token());
			_combine.top += in[i - 1]->get_orig_bytes();
		} 
		if (with & 2) {
			strcpy(_combine.top, in[i]->get_orig_token());
			_combine.top += in[i]->get_orig_bytes();
		} 
		if (with & 1) {
			strcpy(_combine.top, in[i + 1]->get_orig_token());
			_combine.top += in[i + 1]->get_orig_bytes();
		}
		*_combine.top = '\0';
		if (with && (check == false || (check && _lexicon_combine->search(_combine.base) > 0))) {
			if (with & 4) {
				out.pop_back();
				delete in[i - 1];
				in[i - 1] = NULL;
			} 
			if (with & 2) {
				delete in[i];
				in[i] = NULL;
			} 
			if (with & 1) {
				delete in[i + 1];
				in[i + 1] = NULL;
				++i;
			}
			out.push_back(new LexToken(_combine.base, LexToken::attr_cword));
		} else {
			out.push_back(in[i]);
		}
	}
}
