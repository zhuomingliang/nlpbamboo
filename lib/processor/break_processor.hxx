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

#ifndef BREAK_PROCESSOR_HXX
#define BREAK_PROCESSOR_HXX

#include "token_impl.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"

namespace bamboo {


class BreakProcessor: public Processor {
protected:
	ILexicon *_lexicon;
	int _split;
	char *_token;
	int _min_token_length, _max_token_length;
	BreakProcessor();
	bool _can_process(TokenImpl *token) 
	{
		_split = _lexicon->search(token->get_token());
		if ((token->get_length() >= (size_t)_min_token_length)
			&& (token->get_length() <= sizeof(size_t) * 8)
			&& _split)
			return true;
		return false;
	}
	void _process(TokenImpl *token, std::vector<TokenImpl *> &out);

public:
	BreakProcessor(IConfig *config);
	~BreakProcessor();
};

} //namespace bamboo

#endif // BREAK_PROCESSOR_HXX
