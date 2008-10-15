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
#include "simplener_processor.hxx"
#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "bamboo.hxx"

using namespace std;

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(SIMPLENERProcess)

SIMPLENERProcess::SIMPLENERProcess(IConfig *config) {
	const char *s;
	struct stat buf;

	config->get_value("crf_ner2_model", s);
	std::string model_param = std::string("-m ") + std::string(s);
	if(stat(s, &buf)==0) {
		_tagger = CRFPP::createTagger(model_param.c_str());
	} else {
		throw std::runtime_error(std::string("can not load model ") + s + ": " + strerror(errno));
	}
}

SIMPLENERProcess::~SIMPLENERProcess() {
	delete _tagger;
}

void SIMPLENERProcess::process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out) {
	_tagger->clear();

	size_t i, size = in.size();

	for(i=0; i<size; ++i) {
		TokenImpl *token = in[i];
		const char * str = token->get_token();
		_tagger->add(1, &str);
		delete token;
	}

	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");

	enum {max_str_size=128};
	std::string ner_word("");
	ner_word.reserve(max_str_size);
	size = _tagger->size();
	for(i=0; i<size; ++i) {
		const char *tag = _tagger->y2(i);
		if(*tag != 'O') ner_word += _tagger->x(i, 0);

		if( (*tag=='E'||*tag=='S'||*tag=='O') && ner_word.size()>0) {
			out.push_back(new TokenImpl(ner_word.c_str()));
			ner_word.clear();
		}

	}
}


} //namespace bamboo
