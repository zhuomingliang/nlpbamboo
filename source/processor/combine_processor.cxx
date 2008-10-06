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
#include "combine_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <iostream>

PROCESSOR_MAGIC
PROCESSOR_MODULE(CombineProcessor)

CombineProcessor::CombineProcessor(IConfig *config)
{
	const char *s;

	config->get_value("lexicon_combine", s);
	config->get_value("max_token_length", _max_token_length);
	_lexicon = LexiconFactory::load(s);

	_token = new char[(_max_token_length << 2) + 1]; /* x4 for unicode */
	_combine = new char[(_max_token_length << 2) + 1]; /* x4 for unicode */
}

CombineProcessor::~CombineProcessor()
{
	delete []_combine;
	delete []_token;
	delete _lexicon;
}

void CombineProcessor::process(std::vector<LexToken *> &in, std::vector<LexToken *> &out)
{
	size_t i, j, length, num = 0;

	if (in.empty()) return;
	length = in.size();
	for (i = 0; i < length; i++) {
		*_combine = '\0';
		*_token = '\0';
		for (j = i; j < length; j++) {
			if (utf8::length(_combine) + in[j]->get_length() <= (unsigned int)_max_token_length) {
				strcpy(_combine + strlen(_combine), in[j]->get_token());
				if (_lexicon->search(_combine) > 0) {
					strcpy(_token, _combine);
					num = j;
				}
			}
		}
		if (*_token) {
			out.push_back(new LexToken(_token, LexToken::attr_cword));
			for (j = i; j <= num; j++)
				delete in[j];
			i = num;
		} else {
			out.push_back(in[i]);
		}
	}
}
