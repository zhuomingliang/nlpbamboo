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
#include "neras_processor.hxx"
#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "token_impl.hxx"

using namespace std;

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(NERASProcessor)

NERASProcessor::NERASProcessor(IConfig *config)
	:_tagger(NULL), _ner_type(NULL)
{
	_config = config;
}

void NERASProcessor::init(const char * ner_type) {
	const char * model;
	struct stat buf;
	enum {max_model_name_len = 32};
	char *model_name = new char[max_model_name_len];
	snprintf(model_name, max_model_name_len, "crf_neras_%s_model", ner_type);
	_config->get_value(model_name, model);

	std::string model_param = std::string("-m ") + std::string(model);
	if(stat(model, &buf)==0) {
		_tagger = CRFPP::createTagger(model_param.c_str());
	} else {
		throw std::runtime_error(std::string("can not load model ") + model + ": " + strerror(errno));
	}

	_ner_type = strdup(ner_type);
	delete [] model_name;
}

NERASProcessor::~NERASProcessor() {
	if(_tagger) delete _tagger;
	if(_ner_type) free(_ner_type);
}

void NERASProcessor::process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out) {

	size_t i, size = in.size();
	_tagger->clear();

	TokenImpl *token;
	char pos_str[3] = {0};
	for(i=0; i<size; ++i) {
		token = in[i];
		unsigned short POS = token->get_pos();
		if(token->get_pos() > 256) {
			pos_str[0] = 'M';
			pos_str[1] = '\0';
		} else {
			pos_str[0] = POS % 256;
			pos_str[1] = '\0';
		}
		const char *data[] = {token->get_token(), pos_str};
		_tagger->add(2, data);
	}


	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");

	enum { begin_ner, end_ner, non_ner } state;
	for(i=0; i<size; ++i) {
		token = in[i];
		const char *ner_tag = _tagger->y2(i);
		const char *seg_tag = _tagger->x(i, 1);
		int attr = token->get_attr();

		if(!strcmp(ner_tag, "B")) {
			state = begin_ner;
		} else if(*ner_tag == 'S') {
			state = end_ner;
		} else if(*ner_tag == 'E') {
			state = end_ner;
		} else if(*ner_tag == 'O') {
			state = non_ner;
		} else {
		}

		if(state != non_ner) {
			_result.append(token->get_token());
			_result_orig.append(token->get_orig_token());

			if(attr==TokenImpl::attr_unknow) attr = TokenImpl::attr_cword;
			if(attr==TokenImpl::attr_alpha || attr==TokenImpl::attr_number || attr==TokenImpl::attr_punct)	seg_tag = "S";
			if(*seg_tag=='S' || *seg_tag=='E') {
				out.push_back(new TokenImpl(_result.c_str(), _result_orig.c_str(), attr));
				out.back()->set_pos(_ner_type);
				_result.clear();
				_result_orig.clear();
			}
		} else {
			_result.clear();
			_result_orig.clear();
		}

		if(state == end_ner) {
			out.push_back(new TokenImpl(",", attr));
			out.back()->set_pos("w");
		}
	}
}


} //namespace bamboo
