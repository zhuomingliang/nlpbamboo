/*
 * Copyright (c) 2008, weibingzheng@gmail.com
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
 * THIS SOFTWARE IS PROVIDED BY weibingzheng@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL weibingzheng@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef CRF_NER_NS_PROCESSOR_HXX
#define CRF_NER_NS_PROCESSOR_HXX

#include "token_impl.hxx"
#include "processor.hxx"
#include "lexicon_factory.hxx"
#include "ilexicon.hxx"
#include <sstream>
#include <crfpp.h>

namespace bamboo {


enum {
	LOC_OTHER=0,
	LOC_SUFFIX=1,
	LOC_DIRECTION=2,
	LOC_CNDIGIT=3,
	LOC_ARABIC=4
};

static const char * loc_label[] = {
	"5",
	"1",
	"2",
	"3",
	"4"
};

class CRFNSProcessor: public Processor {
protected:
	CRFPP::Tagger *_tagger;
	const char * _ner_type;
	std::string _result;
	std::string _result_orig;
	bamboo::ILexicon * _suffix_dict;
	int _ner_output_type;
	
	CRFNSProcessor();
	bool _can_process(TokenImpl *token) {return true;}
	void _process(TokenImpl *token, std::vector<TokenImpl *> &out) {}
	void _process_ner(std::vector<TokenImpl *> &in, size_t offset, std::vector<TokenImpl *> &out);
	const char * _get_label(const char * token);

public:
	CRFNSProcessor(IConfig *config);
	~CRFNSProcessor();

	void process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out);
};

} //namespace bamboo

#endif // CRF_NER_NS_PROCESSOR_HXX
