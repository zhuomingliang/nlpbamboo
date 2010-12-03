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

#include <cassert>
#include <iostream>

#include "lexicon_factory.hxx"
#include "single_combine_processor.hxx"
#include "utf8.hxx"

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(SingleCombineProcessor)

SingleCombineProcessor::SingleCombineProcessor(IConfig *config)
	:_combine_koko(0), _combine_forward(0), _combine_backward(0),
	 _combine_neighbor(0)
{
	const char *s;

	config->get_value("combine_koko", _combine_koko);
	config->get_value("combine_forward", _combine_forward);
	config->get_value("combine_backward", _combine_backward);
	config->get_value("combine_neighbor", _combine_neighbor);
	config->get_value("single_combination_lexicon", s);
	if (*s == '\0')
		throw std::runtime_error("single_combination_lexicon is null");
	_lexicon_combine = LexiconFactory::load(s);
	config->get_value("number_trailing_lexicon", s);
	if (*s == '\0')
		throw std::runtime_error("number_trailing_lexicon is null");
	_lexicon_number_trailing = LexiconFactory::load(s);
}

SingleCombineProcessor::~SingleCombineProcessor()
{
	delete _lexicon_combine;
	delete _lexicon_number_trailing;
}

void SingleCombineProcessor::_make_combine(std::vector<TokenImpl *> &in, int i, int with)
{
	/* with: a bitmap indicates copy range. 
	 * binary  :  0        0      0
	 * position:  ^(i-1)   ^(i)   ^(i + 1)
	 * */
	_combine.erase();
	if (with & 4) _combine.append(in[i - 1]->get_orig_token());
	if (with & 2) _combine.append(in[i]->get_orig_token());
	if (with & 1) _combine.append(in[i + 1]->get_orig_token());
}

int SingleCombineProcessor::_single_combine(size_t i, size_t size, 
		std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out)
{
	int match = 0;

	if (_combine_neighbor && !match && i > 0 && i + 1 < size 
		&& in[i - 1] && in[i + 1]) {
		_make_combine(in, i, 7);
		if (_lexicon_combine->search(_combine.c_str()) > 0) match = 7; 
	}
	if (_combine_forward && !match && i > 0 && in[i - 1]) {
		_make_combine(in, i, 6);
		if (_lexicon_combine->search(_combine.c_str()) > 0) match = 6;
	}
	if (_combine_backward && !match && i + 1 < size && in[i + 1]) {
		_make_combine(in, i, 3);
		if (_lexicon_combine->search(_combine.c_str()) > 0) match = 3;
	}
	if (_combine_koko && !match && i > 0 && in[i - 1] && in[i - 1]->get_length() == 1
		&& strcmp(in[i]->get_token(), in[i - 1]->get_token()) == 0) {
		_make_combine(in, i, 6);
		match = 6;
	}
	return match;
}

int SingleCombineProcessor::_number_trailing_combine(size_t i, size_t size, 
		std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out)
{
	int match = 0;
	if (i + 1 < size && in[i + 1]
			  && in[i]->get_attr() == TokenImpl::attr_number 
			  && _lexicon_number_trailing->search(in[i + 1]->get_token()))
	{
		_make_combine(in, i, 3);
		match = 3;
	}
	return match;
}

void SingleCombineProcessor::process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out)
{
	size_t i, size, length, match;
	int attr;

	size = in.size();
	for (i = 0; i < size; i++) {
		if (in[i] == NULL) continue;
		match = 0;
		length = in[i]->get_length();
		attr = in[i]->get_attr();
		
		if (!match)
			match = _number_trailing_combine(i, size, in, out);

		if (!match && length <= 2) 
			match = _single_combine(i, size, in, out);
		
		if (match) {
			if (match & 4) {
				out.pop_back();
				delete in[i - 1];
				in[i - 1] = NULL;
			} 
			if (match & 2) {
				delete in[i];
				in[i] = NULL;
			} 
			if (match & 1) {
				delete in[i + 1];
				in[i + 1] = NULL;
			}
			out.push_back(new TokenImpl(_combine.c_str(), attr));
		} else {
			out.push_back(in[i]);
		}
	}
}


} //namespace bamboo

