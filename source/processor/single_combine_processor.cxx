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
	size_t i, j, size, num, length;
	int with;
	bool check;
	const char *s;

	size = in.size();
	for (i = 0; i < size; i++) {
		check = false;
		with = 0;
		if (in[i]) {
			length = in[i]->get_length();
			if (i + 1 < size && in[i + 1]
					  && in[i]->get_attr() == LexToken::attr_number 
					  && _lexicon_number_trailing->search(in[i + 1]->get_token()))
			{
				with = 3;
				check = false;
			} else if (length == 1 && i + 1 < size && in[i + 1]) {
				with = 3;
				check = true;
			} else if (length == 1 && i > 0 && i + 1 < size && in[i - 1] && in[i + 1]) {
				with = 7;
				check = true;
			}
		}

//		_combine.top = _combine.base;
		_combine.erase();
		if (with & 4) {
//			strcpy(_combine.top, in[i - 1]->get_orig_token());
//			_combine.top += in[i - 1]->get_orig_bytes();
			_combine.append(in[i - 1]->get_orig_token());
		} 
		if (with & 2) {
//			strcpy(_combine.top, in[i]->get_orig_token());
//			_combine.top += in[i]->get_orig_bytes();
			_combine.append(in[i]->get_orig_token());
		} 
		if (with & 1) {
//			strcpy(_combine.top, in[i + 1]->get_orig_token());
//			_combine.top += in[i + 1]->get_orig_bytes();
			_combine.append(in[i + 1]->get_orig_token());
		}
//		*_combine.top = '\0';
		s = _combine.c_str();
		if (with && (check == false || (check && _lexicon_combine->search(s) > 0))) {
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
			out.push_back(new LexToken(s, LexToken::attr_cword));
		} else {
			out.push_back(in[i]);
		}
	}
}
