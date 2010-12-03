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
#include "crf_seg4ner_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <cstdio>
#include <stdexcept>

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(CRFSeg4nerProcessor)

CRFSeg4nerProcessor::CRFSeg4nerProcessor(IConfig *config)
	:_output_type(1)
{
	const char *s;
	_token = new char[8];
	struct stat st;

	config->get_value("crf_seg_model", s);

	std::string model;
#ifdef DEBUG
	model = std::string("-n5 -m ") + std::string(s);
#else
	model = std::string("-m ") + std::string(s);
#endif
	if (stat(s, &st) == 0) {
		_tagger = CRFPP::createTagger(model.c_str());
	} else {
		throw std::runtime_error(std::string("can not load model ") + s + ": " + strerror(errno));
	}
}

void CRFSeg4nerProcessor::init(const char *type) {
	if(*type == '1')
		_output_type = 1;
	else
		_output_type = 0;
}

CRFSeg4nerProcessor::~CRFSeg4nerProcessor()
{
	delete []_token;
	delete _tagger;
}

inline const char *CRFSeg4nerProcessor::_get_crf2_tag(int attr) {
	switch(attr) {
	case TokenImpl::attr_number:
	case TokenImpl::attr_alpha:
		return "ASCII";
	case TokenImpl::attr_punct:
		return "PUNC";
	case TokenImpl::attr_cword:
		return "CN";
	default:
		return "CN";
	}
}

void CRFSeg4nerProcessor::process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out) {
	size_t i, offset, size = in.size();

	_tagger->clear();
	for (i = 0; i < size; ++i) {
		TokenImpl *cur_tok = in[i];
		const char *tok_str = cur_tok->get_token();
		if(cur_tok->get_attr() == TokenImpl::attr_punct) tok_str = cur_tok->get_orig_token();

		if(cur_tok->get_pos() != 0) {
			offset = i - _tagger->size();
			_crf2_tagger(in, offset, out);
			_tagger->clear();

			/*if(_output_type==1)
				cur_tok->set_pos("S");*/

			out.push_back(cur_tok);
			continue;
		} else {
			const char *data[] = {tok_str, _get_crf2_tag(cur_tok->get_attr())};
			_tagger->add(2, data);
		}

		if(*tok_str=='!' || *tok_str=='?' || *tok_str==';' || !strcmp(tok_str, "。")) {
			offset = i - _tagger->size() + 1;
			_crf2_tagger(in, offset, out);
			_tagger->clear();
		}
	}
	offset = i - _tagger->size();
	_crf2_tagger(in, offset, out);
}

void CRFSeg4nerProcessor::_crf2_tagger(std::vector<TokenImpl *> &in, size_t offset, std::vector<TokenImpl *> &out) {
	if(_tagger->size()==0) return;
	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");

	_result.clear();
	_result_orig.clear();

	for (size_t i = 0; i < _tagger->size(); ++i) {
		TokenImpl *cur_tok = in[offset+i];
		const char * tag = _tagger->y2(i);
		if(_output_type==1) {
			cur_tok->set_pos(tag);
			out.push_back(cur_tok);
		} else {
			_result.append(_tagger->x(i, 0));
			_result_orig.append(cur_tok->get_orig_token());
			int attr = cur_tok->get_attr();
			if(attr==TokenImpl::attr_unknow) attr = TokenImpl::attr_cword;
			if(attr==TokenImpl::attr_alpha || attr==TokenImpl::attr_number || attr==TokenImpl::attr_punct)	tag = "S";
			if (*tag=='S' || *tag=='E') {
				out.push_back(new TokenImpl(_result.c_str(), _result_orig.c_str(), attr));
				_result.clear();
				_result_orig.clear();
			}
			delete cur_tok;
		}
	}

	_tagger->clear();
}


} //namespace bamboo
