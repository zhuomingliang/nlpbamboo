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
#include "ugm_seg_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <iostream>

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(UnigramProcessor)

UnigramProcessor::UnigramProcessor(IConfig *config)
{
	const char *s;

	config->get_value("ele_lambda", _lambda);
	config->get_value("unigram_lexicon", s);
	if (*s == '\0')
		throw std::runtime_error("unigram_lexicon is null");
	config->get_value("max_token_length", _max_token_length);
	_lexicon = LexiconFactory::load(s);

	_token = new char[(_max_token_length << 2) + 1]; /* x4 for unicode */
}

UnigramProcessor::~UnigramProcessor()
{
	delete []_token;
	delete _lexicon;
}

void UnigramProcessor::_process(TokenImpl *token, std::vector<TokenImpl *> &out)
{
	size_t i, j, k, length, max_token_length, *backref;
	double *score, lp;
	size_t num_terms, num_types;
	const char *s;

	s = token->get_token();
	num_terms = _lexicon->sum_value();
	num_types = _lexicon->num_insert();

	length = token->get_length();
	score = new double[length + 1];
	backref = new size_t[length + 1];

	for (i = 0; i <= length; i++) {
		score[i] = -1e300;
		backref[i] = 0;
	}

	/* Calculate score using DP */
	score[0] = 0;
	for (i = 0; i < length; i++) {
		max_token_length = (_max_token_length  + i < length)?_max_token_length:length - i;
		bool found = false;
		for (j = 1; j <= max_token_length; j++) {
			k = utf8::sub(_token, s, i, j);
			int v = _lexicon->search(_token);
			if (v > 0) {
				lp = _ele_estimate(v, num_terms, num_types);
				if (score[i + j] < score[i] + lp) {
					score[i + j] = score[i] + lp;
					backref[i + j] = i;
					found = true;
				}
			}
		}

		if (!found) {
			lp = _ele_estimate(0, num_terms, num_types);
			j = 1;
			if (score[i + j] < score[i] + lp) {
				score[i + j] = score[i] + lp;
				backref[i + j] = i;
			}
		}
	}

	assert(stack.empty() == true);
	for (k = 0, i = length; i > 0;) {
		k = utf8::sub(_token, s, backref[i], i - backref[i]);
		stack.push(new TokenImpl(_token, TokenImpl::attr_cword));
		i = backref[i];
	}
	while(!stack.empty()) {
		out.push_back(stack.top());
		stack.pop();
	}
	delete []score;
	delete []backref;
}


} //namespace bamboo

