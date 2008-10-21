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
 * DISCLAIMED. IN NO EVENT SHALL {detrox,weibingzheng}@gmail.com BE LIABLE FOR ANY
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

#include "token_impl.hxx"
#include "prepare_processor.hxx"

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(PrepareProcessor)

PrepareProcessor::PrepareProcessor(IConfig *config)
	:_characterize(0)
{
	config->get_value("prepare_characterize", _characterize);
}

void PrepareProcessor::_process(TokenImpl *token, std::vector<TokenImpl *> &out)
{
	const char *s;
	char uch[8], cch;
	size_t step;
	TokenImpl::attr_t attr;
	enum {
		PS_UNKNOW = 0,
		PS_ALPHA,
		PS_NUMBER,
		PS_PUNCT,
		PS_WHITESPACE,
		PS_IDENT,
		PS_END
	} state, parent;

	struct {
		char *base;
		char *top;
	} sbc, dbc;

	s = token->get_token();
	sbc.base = new char [token->get_bytes() + 1];
	sbc.top = sbc.base;
	dbc.base = new char [token->get_bytes() + 1];
	dbc.top = dbc.base;

	for (cch = '\0', state = PS_UNKNOW; ; s += step, cch = '\0') {
		step = utf8::first(s, uch);
		cch = utf8::sbc2dbc(uch, step);
		parent = state;

#define isconcat(c) ((c) == '-' || (c) == '_')

		/* state transitions */
		if (isalpha(cch)) state = PS_ALPHA;
		else if (cch == '.' && state == PS_NUMBER) state = PS_NUMBER;
		else if (isdigit(cch)) state = PS_NUMBER;
		else if (isconcat(cch)) state = PS_IDENT;
		else if (ispunct(cch)) state = PS_PUNCT;
		else if (isspace(cch)) state = PS_WHITESPACE;
		else if (*uch == '\0') state = PS_END;
		else state = PS_UNKNOW;

		
		if ((parent == PS_ALPHA || parent == PS_NUMBER)
			&& (state == PS_ALPHA || state == PS_NUMBER || state == PS_IDENT)
			&& state != parent) {
			parent = state = PS_IDENT;
		} else if (parent == PS_IDENT && (state == PS_ALPHA || state == PS_NUMBER || state == PS_IDENT))
			state = PS_IDENT;

		if (state != parent || ( _characterize && state == PS_UNKNOW) ){
			if (parent == PS_WHITESPACE) {
				// do nothing
			} else if (sbc.top > sbc.base) {
				switch (parent) {
					case PS_IDENT:
					case PS_ALPHA: attr = TokenImpl::attr_alpha; break;
					case PS_NUMBER: attr = TokenImpl::attr_number; break;
					case PS_PUNCT: attr = TokenImpl::attr_punct; break;
					default: attr = TokenImpl::attr_unknow; break;
				}
				*(sbc.top) = '\0';
				*(dbc.top) = '\0';
				if (dbc.top > dbc.base)
					out.push_back(new TokenImpl(dbc.base, sbc.base, attr));
				else
					out.push_back(new TokenImpl(sbc.base, attr));
				sbc.top = sbc.base;
				dbc.top = dbc.base;
			}
			if (state == PS_END) break;
		}

		if (state != PS_WHITESPACE) {
			strcpy(sbc.top, uch);
			sbc.top += step;
			if (cch) *(dbc.top++) = cch;
		}
	}
#undef isconcat	
	delete []sbc.base;
	delete []dbc.base;
}


} //namespace bamboo
