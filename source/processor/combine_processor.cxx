#include "lexicon_factory.hxx"
#include "combine_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <iostream>

CombineProcessor::CombineProcessor(IConfig *config)
{
	const char *s;

	config->get_value("lexicon_combine", s);
	config->get_value("max_token_length", _max_token_length);
	_lexicon = LexiconFactory::load(s);

	_token = new char[_max_token_length << 2 + 1]; /* x4 for unicode */
	_combine = new char[_max_token_length << 2 + 1]; /* x4 for unicode */
}

CombineProcessor::~CombineProcessor()
{
	delete []_combine;
	delete []_token;
	delete _lexicon;
}

void CombineProcessor::process(std::vector<LexToken *> &in, std::vector<LexToken *> &out)
{
	size_t i, j, length, num;

	if (in.empty()) return;
	length = in.size();
	for (i = 0; i < length; i++) {
		*_combine = '\0';
		*_token = '\0';
		for (j = i; j < length; j++) {
			if (utf8::length(_combine) + utf8::length(in[j]->get_token()) <= _max_token_length) {
				strcpy(_combine + strlen(_combine), in[j]->get_token());
				if (_lexicon->search(_combine) > 0) {
					strcpy(_token, _combine);
					num = j;
				}
			}
		}
		if (*_token) {
			out.push_back(new LexToken(_token, LexToken::attr_cword));
			for (j = i; j < num; j++)
				delete in[j];
			i = num - 1;
		} else {
			out.push_back(in[i]);
		}
	}
}
