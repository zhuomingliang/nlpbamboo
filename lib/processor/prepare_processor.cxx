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
	:_characterize(0), _concat(0)
{
	config->get_value("prepare_characterize", _characterize);
	config->get_value("concat_hyphen", _concat);
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
        PS_BEGIN,   // sentinel state before first char
		PS_END,     // sentinel state after last char
	} state, last_state;

	struct {
		char *base;
		char *top;
	} dbc, sbc;

	s = token->get_token();
	dbc.base = new char [token->get_bytes() + 1];
	dbc.top = dbc.base;
	sbc.base = new char [token->get_bytes() + 1];
	sbc.top = sbc.base;

    	for (last_state = PS_BEGIN; ; s += step) {
		step = utf8::first(s, uch);
		cch = utf8::dbc2sbc(uch, step);

#define isconcat(c) ((c) == '-' || (c) == '_')

		/* state transitions */
		if (isalpha(cch)) state = PS_ALPHA;
		else if (cch == '.' && last_state == PS_NUMBER) state = PS_NUMBER;
		else if (isdigit(cch)) state = PS_NUMBER;
		else if (_concat && isconcat(cch)) state = PS_IDENT;
		else if ( ispunct(cch) || !strcmp(uch, "。") || !strcmp(uch, "、") || !strcmp(uch, "《") || !strcmp(uch, "》") || !strcmp(uch, "•") ) state = PS_PUNCT;
		else if (isspace(cch)) state = PS_WHITESPACE;
		else if (*uch == '\0') state = PS_END;
		else state = PS_UNKNOW;
		
		if (state != last_state
            && (last_state == PS_ALPHA || last_state == PS_NUMBER)
			&& (state == PS_ALPHA || state == PS_NUMBER || state == PS_IDENT)
		) {
			last_state = state = PS_IDENT;
		} else if (last_state == PS_IDENT && (state == PS_ALPHA || state == PS_NUMBER)) {
			state = PS_IDENT;
        }

		if (state != last_state || ( _characterize && state == PS_UNKNOW) 
		   || state == PS_PUNCT || state == PS_WHITESPACE)
		{
			if (dbc.top > dbc.base) {
				switch (last_state) {
					case PS_IDENT: /* no break here */
					case PS_ALPHA: attr = TokenImpl::attr_alpha; break;
					case PS_NUMBER: attr = TokenImpl::attr_number; break;
					case PS_WHITESPACE: attr = TokenImpl::attr_whitespace; break;
					case PS_PUNCT: attr = TokenImpl::attr_punct; break;
					default: attr = TokenImpl::attr_unknow; break;
				}
				*(dbc.top) = '\0';
				*(sbc.top) = '\0';
				if (sbc.top > sbc.base)
					out.push_back(new TokenImpl(sbc.base, dbc.base, attr));
				else
					out.push_back(new TokenImpl(dbc.base, attr));
                
				dbc.top = dbc.base;
				sbc.top = sbc.base;
			}
			if (state == PS_END) break;
		}

        strcpy(dbc.top, uch);
        dbc.top += step;
        if (dbc.top >= dbc.base + MAX_TOKEN_BUFFER)
            state = PS_UNKNOW;
        if (cch) {
            *(sbc.top++) = cch;
        }
        last_state = state;
	}
#undef isconcat	
	delete []dbc.base;
	delete []sbc.base;
}


} //namespace bamboo
