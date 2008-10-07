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

#ifndef CNLEXIZER_HXX
#define CNLEXIZER_HXX

#include <stdexcept>
#include <cstring>
#include <vector>

#include "lexicon_factory.hxx"
#include "config_factory.hxx"
#include "processor.hxx"
#include "lex_token.hxx"

class CNLexizer {
private:
	
public:
	void process(std::vector<LexToken> &vec, const char *s);
	size_t process(char *t, const char *s);
	CNLexizer(const char *file);
	~CNLexizer();
protected:
	IConfig *_config;
	const char *processor_root;
	std::vector<std::string> _process_chain;
	std::vector<LexToken *> _token_fifo[2];
	std::vector<LexToken *> *_in, *_out, *_swap;
	std::vector<Processor *> _processors;
	std::vector<void *> _dl_handles;
	typedef Processor* (*_create_processor_t)(IConfig *);
	inline void _lazy_create_config(const char *);
	inline void _process(const char *s);
	int _verbose;
#ifdef TIMING
	size_t _timing_process[128];
#endif
};

#endif
