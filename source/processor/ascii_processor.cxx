#include <ctype.h>
#include <iostream>

#include "lex_token.hxx"
#include "ascii_processor.hxx"

AsciiProcessor::AsciiProcessor(IConfig *config)
{
	const char *s;

	config->get_value("chinese_number_end", s);
	_load_lexicon(_chinese_number_end, s);
	config->get_value("chinese_punctuation", s);
	_load_lexicon(_chinese_punctuation, s);
	config->get_value("chinese_number", s);
	_load_lexicon(_chinese_number, s);
}

void AsciiProcessor::_process(LexToken *token, std::vector<LexToken *> &out)
{
	const char *s;
	char uch[4], *stack, *top;
	size_t step;
	enum {
		state_unknow = 0,
		state_alpha,
		state_number,
		state_punctuation,
		state_end
	} state, last;

	s = token->get_token();
	stack = new char [token->get_bytes() + 1];
	*stack = '\0';
	top = stack;
	state = state_unknow;

	while (true) {
		last = state;
		step = utf8::first(s, uch);
		if (isalpha(*uch)) state = state_alpha;
		else if (isdigit(*uch)) state = state_number;
		else if (*uch == '.' && last == state_number) state = state_number; /* should before punctuation */
		else if (_chinese_punctuation.search(uch) > 0 || ispunct(*uch)) state = state_punctuation;
		else if (_chinese_number.search(uch) > 0) state = state_number;
		else if (_chinese_number_end.search(uch) > 0 && last == state_number) state = state_number;
		else if (*uch == '\0') state = state_end;
		else state = state_unknow;
		if (last != state && *stack) {
			LexToken::attr_t attr = LexToken::attr_unknow;
			if (last == state_alpha) attr = LexToken::attr_alpha;
			else if (last == state_number) attr = LexToken::attr_number;
			*top = '\0';
			out.push_back(new LexToken(stack, attr));
			top = stack;
			*stack = '\0';
			if (state == state_end) break;
		}
		strcpy(top, uch);
		top += step;
		s += step;
	}
	delete []stack;
}
