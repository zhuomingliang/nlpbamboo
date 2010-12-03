/*
 * Copyright (c) 2008, detrox@gmail.com
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
 * THIS SOFTWARE IS PROVIDED BY detrox@gmail.com ''AS IS'' AND ANY
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

#include "lexicon_factory.hxx"
#include "maxforward_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <iostream>

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(MaxforwardProcessor)

MaxforwardProcessor::MaxforwardProcessor(IConfig *config)
	:_token(NULL)
{
	const char *s;

	config->get_value("unigram_lexicon", s);
	if (*s == '\0')
		throw std::runtime_error("unigram_lexicon is null");
	config->get_value("max_token_length", _max_token_length);
	_lexicon = LexiconFactory::load(s);

	_token = new char[(_max_token_length << 2) + 1]; /* x4 for unicode */
}

MaxforwardProcessor::~MaxforwardProcessor()
{
	delete []_token;
	delete _lexicon;
}

void MaxforwardProcessor::_process(TokenImpl *token, std::vector<TokenImpl *> &out)
{
	size_t length, max_token_length, i, j, k;
	const char *s;

	s = token->get_token();
	length = token->get_length();
	for (i = 0; i < length; i++) {
		max_token_length = ((unsigned int)_max_token_length + i< length)?_max_token_length:length - i;
		for (k = 0, j = max_token_length; j > 0; j--) {
			k = utf8::sub(_token, s, i, j);
			if (_lexicon->search(_token) > 0) break;
		}
		if (j == 0) {j = 1;}
		out.push_back(new TokenImpl(_token, TokenImpl::attr_cword));
		i = i + j - 1;
	}
}


} //namespace bamboo

