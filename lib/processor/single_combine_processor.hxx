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

#ifndef SINGLE_COMBINE_PROCESSOR_HXX
#define SINGLE_COMBINE_PROCESSOR_HXX

#include "token_impl.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"
#include "utf8.hxx"

namespace bamboo {


class SingleCombineProcessor: public Processor {
protected:
	ILexicon *_lexicon_combine, *_lexicon_number_trailing;
	std::string _combine;
	SingleCombineProcessor();
	int _combine_koko, _combine_forward, _combine_backward, _combine_neighbor;
	bool _can_process(TokenImpl *token) {return true;}
	void _process(TokenImpl *token, std::vector<TokenImpl *> &out) {};
	inline int _number_trailing_combine
		(size_t i, size_t size, std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out);
	inline int _single_combine
		(size_t i, size_t size, std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out);
	inline void _make_combine(std::vector<TokenImpl *> &in, int i, int with);
public:
	void process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out);
	SingleCombineProcessor(IConfig *config);
	~SingleCombineProcessor();
};

} //namespace bamboo

#endif // SINGLE_COMBINE_PROCESSOR_HXX
