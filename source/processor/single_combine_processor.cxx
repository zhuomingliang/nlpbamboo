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
	_lexicon = LexiconFactory::load(s);
	config->get_value("max_token_length", _max_token_length);
	config->get_value("chinese_number_end", s);
	_load_lexicon(_chinese_number_end, s);

	_combine.base = new char[_max_token_length << 2 + 1]; /* x4 for unicode */
}

SingleCombineProcessor::~SingleCombineProcessor()
{
	delete []_combine.base;
	delete _lexicon;
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
		if (in[i]->get_length() == 1) {
			if (i > 0 && in[i - 1]->get_attr() == LexToken::attr_number 
					  && _chinese_number_end.search(in[i]->get_token()))
			{
				with = 6;
				check = false;
			} else if (i > 0 && in[i - 1]->get_length() == 1) {
				with = 6;
				check = true;
			} else if (i > 0 && i + 1 < size 
					   && in[i - 1]->get_length() == 1 
					   && in[i + 1]->get_length() == 1) {
				with = 7;
				check = true;
			}
		}

		_combine.top = _combine.base;
		if (with & 4) {
			strcpy(_combine.top, in[i - 1]->get_orig_token());
			_combine.top += in[i - 1]->get_bytes();
		} 
		if (with & 2) {
			strcpy(_combine.top, in[i]->get_orig_token());
			_combine.top += in[i]->get_bytes();
		} 
		if (with & 1) {
			strcpy(_combine.top, in[i + 1]->get_orig_token());
			_combine.top += in[i + 1]->get_bytes();
		}
		*_combine.top = '\0';
		if (with && (check == false || (check && _lexicon->search(_combine.base) > 0))) {
			std::cout << "combine = " << _combine.base << std::endl;
			if (with & 4) {
				out.pop_back();
				delete in[i - 1];
			} 
			if (with & 2) {
				delete in[i];
			} 
			if (with & 1) {
				delete in[i + 1];
				++i;
			}
			out.push_back(new LexToken(_combine.base, LexToken::attr_cword));
		} else {
			out.push_back(in[i]);
		}
	}
}
