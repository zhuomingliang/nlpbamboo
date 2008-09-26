#include <ctype.h>
#include <iostream>

#include "lex_token.hxx"
#include "prepare_processor.hxx"

PrepareProcessor::PrepareProcessor(IConfig *config)
{
	const char *s;

	config->get_value("chinese_punctuation", s);
	_load_lexicon(_chinese_punctuation, s);
	config->get_value("chinese_number", s);
	_load_lexicon(_chinese_number, s);
	config->get_value("chinese_alpha", s);
	_load_lexicon(_chinese_alpha, s);
}

void PrepareProcessor::_process(LexToken *token, std::vector<LexToken *> &out)
{
	const char *s;
	char uch[8], cch;
	size_t step;
	int id;
	LexToken::attr_t attr;
	enum {
		state_unknow = 0,
		state_alpha,
		state_number,
		state_punctuation,
		state_end
	} state, last;

	struct {
		char *base;
		char *top;
	} chinese, english;

	s = token->get_token();
	chinese.base = new char [token->get_bytes() + 1];
	chinese.top = chinese.base;
	english.base = new char [token->get_bytes() + 1];
	english.top = english.base;

	state = state_unknow;
	while (true) {
		last = state;
		step = utf8::first(s, uch);
		if (isalpha(*uch)) state = state_alpha;
		else if (isdigit(*uch)) state = state_number;
		else if (ispunct(*uch)) state = state_punctuation;
		else if ((id = _chinese_punctuation.search(uch)) > 0) {
			state = state_punctuation;
		} else if ((id = _chinese_number.search(uch)) > 0) {
			cch = id - 1;
			state = state_number;
		} else if ((id = _chinese_alpha.search(uch)) > 0) {
			cch = id - 1 + (id > 26)?'A':'a';
			state = state_alpha;
		} else if (*uch == '\0') {
			state = state_end;
		} else {
			state = state_unknow;
		}

		if (state == state_unknow) {
			attr = LexToken::attr_unknow;
			out.push_back(new LexToken(uch, attr));
		} else {
			strcpy(chinese.top, uch);
			chinese.top += step;
			*(english.top++) = cch;
			if (last != state && english.top > english.base) {
				switch (last) {
					case state_alpha: attr = LexToken::attr_alpha; break;
					case state_number: attr = LexToken::attr_number; break;
				}
				*(chinese.top) = '\0';
				*(english.top) = '\0';
				out.push_back(new LexToken(english.base, chinese.base, attr));
				chinese.top = chinese.base;
				english.top = english.base;
				if (state == state_end) break;
			}
		}
		s += step;
	}
	delete []chinese.base;
	delete []english.base;
}
