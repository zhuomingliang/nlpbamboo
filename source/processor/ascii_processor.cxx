/*
 * Copyright (c) 2008, detrox@gmail.com, weibingzheng@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY {detrox,weibingzheng}@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL detrox@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <ctype.h>
#include <iostream>

#include "lex_token.hxx"
#include "ascii_processor.hxx"

PROCESSOR_MAGIC
PROCESSOR_MODULE(AsciiProcessor)

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

	/* rewrite state machine */
	for (cch = '\0', state = state_unknow;;s += step, cch = '\0') {
		last = state;
		step = utf8::first(s, uch);
		cch = utf8::to_dbc(uch, step);

		if (isalpha(cch)) state = state_alpha;
		/* chinese decimal point */
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
