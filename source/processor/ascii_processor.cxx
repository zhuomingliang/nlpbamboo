#include <ctype.h>
#include <iostream>

#include "lex_token.hxx"
#include "ascii_processor.hxx"

AsciiProcessor::AsciiProcessor(IConfig *config)
{
	const char *s;

	config->get_value("chinese_punctuation", s);
	_load_lexicon(_chinese_punctuation, s);
	config->get_value("chinese_number", s);
	_load_lexicon(_chinese_number, s);
	config->get_value("chinese_alpha", s);
	_load_lexicon(_chinese_alpha, s);
}

void AsciiProcessor::_process(LexToken *token, std::vector<LexToken *> &out)
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
		state_whitespace,
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

	for (cch = '\0', state = state_unknow;;s += step, cch = '\0') {
		last = state;
		step = utf8::first(s, uch);
		if (isalpha(*uch)) {
			cch = *uch;
			state = state_alpha;
		} else if (isdigit(*uch)) {
			cch = *uch;
			state = state_number;
		} else if (ispunct(*uch)) {
			cch = *uch;
			state = state_punctuation;
		} else if ((id = _chinese_punctuation.search(uch)) > 0) {
			state = state_punctuation;
		} else if ((id = _chinese_number.search(uch)) > 0) {
			cch = id - 1 + '0';
			state = state_number;
		} else if ((id = _chinese_alpha.search(uch)) > 0) {
			cch = (id > 26)?id - 27 + 'A':id - 1 + 'a';
			state = state_alpha;
		} else if (isspace(*uch) || strcmp(uch, "　") == 0) {
			state = state_whitespace;
		} else if (*uch == '\0') {
			state = state_end;
		} else {
			state = state_unknow;
		}

		if (last != state){
			if (last == state_whitespace) {
				// do nothing
			} else if (chinese.top > chinese.base) {
				switch (last) {
					case state_alpha: attr = LexToken::attr_alpha; break;
					case state_number: attr = LexToken::attr_number; break;
					default: attr = LexToken::attr_unknow; break;
				}
				*(chinese.top) = '\0';
				*(english.top) = '\0';
				if (english.top > english.base)
					out.push_back(new LexToken(english.base, chinese.base, attr));
				else
					out.push_back(new LexToken(chinese.base, attr));
				chinese.top = chinese.base;
				english.top = english.base;
			}
			if (state == state_end) break;
		}
		strcpy(chinese.top, uch);
		chinese.top += step;
		if (cch)
			*(english.top++) = cch;
	}
	delete []chinese.base;
	delete []english.base;
}
