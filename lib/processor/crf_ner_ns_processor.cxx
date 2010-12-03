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
#include "crf_ner_ns_processor.hxx"
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
PROCESSOR_MODULE(CRFNSProcessor)

CRFNSProcessor::CRFNSProcessor(IConfig *config)
	:_tagger(NULL), _ner_type("ns"), _suffix_dict(NULL), _ner_output_type(0)
{
	const char * model;
	struct stat buf;
	config->get_value("crf_ner_ns_model", model);

	std::string model_param = std::string("-m ") + std::string(model);
	if(stat(model, &buf)==0) {
		_tagger = CRFPP::createTagger(model_param.c_str());
	} else {
		throw std::runtime_error(std::string("can not load model ") + model + ": " + strerror(errno));
	}

	config->get_value("crf_ner_ns_suffix", model);
	if (*model == '\0')
		throw std::runtime_error("crf_ner_ns_suffix is null");
	_suffix_dict = LexiconFactory::load(model);
	config->get_value("ner_output_type", _ner_output_type);
}

CRFNSProcessor::~CRFNSProcessor() {
	if(_tagger) delete _tagger;
	if(_suffix_dict) delete _suffix_dict;
}

const char * CRFNSProcessor::_get_label(const char * token) {
	int val = _suffix_dict->search(token);
	if(val>0) {
		//assert((val>=1 && val<=4));
		return loc_label[val];
	}

	const char * p = token;
	while(*p) {
		if(*p<'0' || *p>'9')
			return loc_label[LOC_OTHER];
		p++;
	}
	return loc_label[LOC_ARABIC];
}

void CRFNSProcessor::process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out) {
	size_t i, offset, size = in.size();
	char pos_str[3] = {0};

	_tagger->clear();
	for (i = 0; i < size; ++i) {
		TokenImpl *token = in[i];
		const char *tok_str = token->get_token();
		unsigned short POS = token->get_pos();
		if(token->get_pos() > 256) {
			if(token->get_pos()/256 == 'n') {
				pos_str[0] = 'S';
				pos_str[1] = '\0';
			} else {
				pos_str[0] = 'M';
				pos_str[1] = '\0';
			}
		} else {
			pos_str[0] = POS % 256;
			pos_str[1] = '\0';
		}

		const char * label = _get_label(tok_str);

		if(token->get_attr() == TokenImpl::attr_punct) {
			offset = i - _tagger->size();
			_process_ner(in, offset, out);
			_tagger->clear();

			//out.push_back(cur_tok);
			if(_ner_output_type==1) {
				out.push_back(token);
				out.back()->set_pos((unsigned short)0);
			} else {
				delete token;
			}
			continue;
		} else {
			const char *data[] = {tok_str, pos_str, label};
			_tagger->add(3, data);
		}
	}
	offset = i - _tagger->size();
	_process_ner(in, offset, out);
	_tagger->clear();
}

void CRFNSProcessor::_process_ner(std::vector<TokenImpl *> &in, size_t offset, std::vector<TokenImpl *> &out) {
	size_t i, size = _tagger->size();

	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");

	enum { begin_ner, end_ner, non_ner } state;
	TokenImpl *token;
	std::string seg_res, seg_res_orig;
	for(i=0; i<size; ++i) {
		token = in[offset + i];
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
			if(_ner_output_type==1) {
				seg_res.clear();
				seg_res_orig.clear();
			}
			_result.append(token->get_token());
			_result_orig.append(token->get_orig_token());

			if(attr==TokenImpl::attr_unknow) attr = TokenImpl::attr_cword;
			if(attr==TokenImpl::attr_alpha || attr==TokenImpl::attr_number || attr==TokenImpl::attr_punct)	seg_tag = "S";
			if(*ner_tag=='S' || *ner_tag=='E') {
				out.push_back(new TokenImpl(_result.c_str(), _result_orig.c_str(), attr));
				out.back()->set_pos(_ner_type);
				_result.clear();
				_result_orig.clear();
			}
		} else {
			_result.clear();
			_result_orig.clear();
			if(_ner_output_type==1) {
				seg_res.append(token->get_token());
				seg_res_orig.append(token->get_orig_token());

				if(*seg_tag=='S'||*seg_tag=='E') {
					out.push_back(new TokenImpl(seg_res.c_str(), seg_res_orig.c_str()));
					if(token->get_pos()/256 == 'n')
						out.back()->set_pos(token->get_pos());
					seg_res.clear();
					seg_res_orig.clear();
				}
			}
		}

		/*if(state == end_ner) {
			out.push_back(new TokenImpl(",", attr));
			out.back()->set_pos("w");
		}*/
		delete token;
	}
}

} //namespace bamboo
