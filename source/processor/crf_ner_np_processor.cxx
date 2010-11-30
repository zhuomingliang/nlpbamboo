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

#include "crf_ner_np_processor.hxx"
#include <cassert>
#include <cstdio>
#include <cerrno>
#include <strings.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include "token_impl.hxx"
#include "utf8.hxx"

using namespace std;

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(CRFNPProcessor)

CRFNPProcessor::CRFNPProcessor(IConfig *config)
	:_tagger(NULL), _ner_output_type(0)
{
	const char * model;
	struct stat buf;
	config->get_value("crf_ner_np_model", model);

	std::string model_param = std::string("-m ") + std::string(model);
	if(stat(model, &buf)==0) {
		_tagger = CRFPP::createTagger(model_param.c_str());
	} else {
		throw std::runtime_error(std::string("can not load model ") + model + ": " + strerror(errno));
	}

	config->get_value("ner_output_type", _ner_output_type);
}

CRFNPProcessor::~CRFNPProcessor() {
	if(_tagger) delete _tagger;
}

const char * CRFNPProcessor::_np_label[] = {
	"6",
	"1",
	"2",
	"3",
	"4",
	"5"
};

const char * CRFNPProcessor::_num_end[] = {
	"kg",
	"g",
	"kb",
	"mb",
	"m",
	"gb",
	"w",
	"k",
	"mm",
	"cm",
	"mhz",
	"mah",
	"bit",
	"kbps",
	"bps",
	"gbps",
	"mbps",
	"pixels",
	"pixel",
	"dpi",
	"fps",
	"GBmicroSD",
	NULL
};

const char * CRFNPProcessor::_get_label(const char * token) {
	char uch[8], cch;
	size_t step, length = 0;
	unsigned int type = 0;
	const char * s = token;

	for(cch=0;;s+=step,cch=0) {
		step = utf8::first(s, uch);
		cch = utf8::dbc2sbc(uch, step);

		if(*uch == 0) break;

		if(cch == 0) {
			type |= 0x01;
		} else if(isalpha(cch)) {
			type |= 0x02;
		} else if(isdigit(cch)) {
			type |= 0x04;
		} else if(cch == '-' || cch == '_' || cch == '@') {
			type |= 0x08;
		} else {
			type |= 0x10;
		}

		++length;
	}

	if( type & 0x10 ) {
		return _np_label[NP_OTHER];
	} else if( type & 0x08 ) {
		return _np_label[NP_CON];
	} else if( (type & 0x04) && (type & 0x02)) {
		if(_is_num_expr(token)) return _np_label[NP_OTHER];
		return _np_label[NP_MIX];
	} else if( type & 0x04) {
		if(length <= 3) return _np_label[NP_OTHER];
		return _np_label[NP_DIGIT];
	} else if(type & 0x02) {
		return _np_label[NP_ALPHA];
	}

	return _np_label[NP_CN];
}

unsigned short CRFNPProcessor::_get_ner_label(char ch) {
	unsigned short p = 'n'*256;
	p += ch;
	return p;
}

bool CRFNPProcessor::_is_num_expr(const char *token) {
	size_t i, len = strlen(token);

	const char ** p = _num_end;
	const char * unit = 0, * tmp;
	size_t unit_len = 0;

	for(; *p; p++) {
		unit = *p;
		unit_len = strlen(unit);
		if(unit_len >= len) continue;

		tmp = token + len - unit_len;
		if(strcasecmp(tmp, unit)) continue;

		for(i=0; i<len-unit_len; ++i) {
			if( ! (isdigit(token[i]) || token[i]=='.' || token[i]=='x' || token[i]=='-') ) {
				break;
			}
		}
		if(i == len-unit_len) return true;
	}

	return false;
}

void CRFNPProcessor::process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out) {
	size_t i, offset, size = in.size();
	char pos_str[3] = {0};
	char buf[8];

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

#define is_connector(c)	((c) == '-' || (c) == '_')

		if(token->get_attr() == TokenImpl::attr_punct
			&& !is_connector(*tok_str)) {
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
			std::string tok_to_lower(tok_str);
			transform(tok_to_lower.begin(), tok_to_lower.end(), tok_to_lower.begin(), ::tolower);

			int token_length = utf8::length(tok_to_lower.c_str());
			if(token_length <= 3) token_length = 1;
			else if(token_length >= 7) token_length = 7;
			snprintf(buf, 8, "%d", token_length);

			const char *data[] = {tok_to_lower.c_str(), pos_str, label, buf};
			_tagger->add(4, data);
		}
	}
	offset = i - _tagger->size();
	_process_ner(in, offset, out);
	_tagger->clear();
}

void CRFNPProcessor::_process_ner(std::vector<TokenImpl *> &in, size_t offset, std::vector<TokenImpl *> &out) {
	size_t i, size = _tagger->size();

	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");

	enum { begin_ner, end_ner, non_ner } state;
	TokenImpl *token;
	std::string seg_res, seg_res_orig;
	for(i=0; i<size; ++i) {
		token = in[offset + i];
		const char *ner_tag_orig = _tagger->y2(i);
		const char *seg_tag = _tagger->x(i, 1);
		int attr = token->get_attr();

		char ner_type = *(ner_tag_orig+1);

		const char * ner_tag = ner_tag_orig;
		if(*ner_tag != 'O') ner_tag += 2;

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

			//if(_result.size() > 0) _result.append(" ");
			//if(_result_orig.size() > 0) _result_orig.append(" ");

			_result.append(token->get_token());
			_result_orig.append(token->get_orig_token());

			if(attr==TokenImpl::attr_unknow) attr = TokenImpl::attr_cword;
			if(attr==TokenImpl::attr_alpha || attr==TokenImpl::attr_number || attr==TokenImpl::attr_punct)	seg_tag = "S";
			if(*ner_tag=='S' || *ner_tag=='E') {
				out.push_back(new TokenImpl(_result.c_str(), _result_orig.c_str(), attr));
				out.back()->set_pos(_get_ner_label(ner_type));
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
