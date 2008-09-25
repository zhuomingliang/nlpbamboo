#include <cassert>
#include <iostream>

#include "lexicon_factory.hxx"
#include "single_combine_processor.hxx"
#include "utf8.hxx"

SingleCombineProcessor::SingleCombineProcessor(IConfig *config)
{
	const char *s;

	config->get_value("lexicon_combine", s);
	_lexicon = LexiconFactory::load(s);

	_max_token_length = 2;
	_combine = new char[_max_token_length << 2 + 1]; /* x4 for unicode */
}

SingleCombineProcessor::~SingleCombineProcessor()
{
	delete []_combine;
	delete _lexicon;
}

void SingleCombineProcessor::process(std::vector<LexToken *> &in, std::vector<LexToken *> &out)
{
	size_t i, j, length, num;
	int now, last;

	if (in.empty()) return;
	length = in.size();
	for (i = 0, last = -1; i < length; i++) {
		*_combine = '\0';
		now = in[i]->get_length();
		if (now == 1 && last == 1) {
			strcpy(_combine, in[i - 1]->get_token());
			strcpy(_combine + strlen(_combine), in[i]->get_token());
			if (_lexicon->search(_combine) > 0) {
				out.pop_back();
				out.push_back(new LexToken(_combine, LexToken::attr_cword));
				delete in[i];
				delete in[i + 1];
				++i;
				last = -1;
				continue;
			}
		}
		last = now;
		out.push_back(in[i]);
	}
}
