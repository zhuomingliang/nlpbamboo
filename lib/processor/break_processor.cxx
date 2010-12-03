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
#include "break_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(BreakProcessor)

BreakProcessor::BreakProcessor(IConfig *config)
	:_split(0)
{
	const char *s;

	config->get_value("max_token_length", _max_token_length);
	config->get_value("break_min_length", _min_token_length);
	config->get_value("break_lexicon", s);
	if (*s == '\0')
		throw std::runtime_error("break_lexicon is null");
	_lexicon = LexiconFactory::load(s);
	if (_min_token_length < 2) _min_token_length = 2;
	if (_max_token_length < 1) throw std::runtime_error("max_token_length must greater than 0");
	_token = new char[(_max_token_length << 2) + 1]; /* x4 for unicode */
}

BreakProcessor::~BreakProcessor()
{
	delete _lexicon;
	delete []_token;
}

/*
 * Break Lexicon Format:
 *                      Bitmap Word
 * e.g.
 *      41 中华人民共和国
 *
 * break step:
 *
 * 1. dec(41) = bin(0101001)
 * 
 * 2. alignment:
 *    中 华 人 民 共 和 国
 *    0  1  0  1  0  0  1
 * 
 * 3. break at 华，民，国
 * 
 * 4. generate 中华/人民/共和国
 *
 */

void BreakProcessor::_process(TokenImpl *token, std::vector<TokenImpl *> &out)
{
	size_t length, i, j, mark;
	const char *s;

	s = token->get_token();
	length = token->get_length();
	for (i = 0, j = 0; i < length; i++) {
		mark = 1 << (length - i - 1);
		if (_split & mark) {
			if (i - j + 1 > 0) {
				utf8::sub(_token, s, j, i - j + 1);
				out.push_back(new TokenImpl(_token, TokenImpl::attr_cword));
			}
			j = i + 1;
		}
	}
	if (length - j > 0) {
		utf8::sub(_token, s, j, length - j);
		out.push_back(new TokenImpl(_token, TokenImpl::attr_cword));
	}
}


} //namespace bamboo

