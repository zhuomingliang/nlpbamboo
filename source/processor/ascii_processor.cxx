#include <ctype.h>
#include <iostream>

#include "lex_token.hxx"
#include "ascii_processor.hxx"

AsciiProcessor::AsciiProcessor(IConfig *config)
{
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
		cch = utf8::to_dbc(uch, step);
		if (isalpha(cch)) state = state_alpha;
		else if (cch == '.' && last == state_number) state = state_number;
		else if (isdigit(cch)) state = state_number;
		//else if (strcmp(uch, "。") == 0) {state = state_punctuation; cch = '.';}
		else if (ispunct(cch)) state = state_punctuation;
		else if (isspace(cch)) state = state_whitespace;
		else if (*uch == '\0') state = state_end;
		else state = state_unknow;

		if (last != state){
			if (last == state_whitespace) {
				// do nothing
			} else if (chinese.top > chinese.base) {
				switch (last) {
					case state_alpha: attr = LexToken::attr_alpha; break;
					case state_number: attr = LexToken::attr_number; break;
					case state_punctuation: attr = LexToken::attr_punct; break;
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
		if (state != state_whitespace) {
			strcpy(chinese.top, uch);
			chinese.top += step;
			if (cch) *(english.top++) = cch;
		}
	}
	delete []chinese.base;
	delete []english.base;
}
