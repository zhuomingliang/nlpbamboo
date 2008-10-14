/*
 * Copyright (c) 2008, detrox@gmail.com, weibingzheng@gmail.com
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
 * THIS SOFTWARE IS PROVIDED BY {detrox,weibingzheng}@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL {detrox,weibingzheng}@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "lexicon_factory.hxx"
#include "crf_processor.hxx"
#include "utf8.hxx"

namespace bamboo {


PROCESSOR_MAGIC
PROCESSOR_MODULE(CRFProcessor)

CRFProcessor::CRFProcessor(IConfig *config)
	:_token(NULL), _result(NULL), _result_top(NULL)
{
	const char *s;
	struct stat buf;

	_token = new char[8];
	 _result = (char *)realloc(_result, 4096);
	 _result_size = 4096;
	config->get_value("crf_token_model", s);
	_model_parameter.append("-m ").append(s);
	if (stat(s, &buf) == 0) {
		_tagger = CRFPP::createTagger(_model_parameter.c_str());
	} else {
		throw std::runtime_error(std::string("can not load model ") + s + ": " + strerror(errno));
	}
#ifdef TIMING
	_timing_parser = 0;
#endif
}

CRFProcessor::~CRFProcessor()
{
	free(_result);
	delete []_token;
	delete _tagger;
#ifdef TIMING
	std::cerr << "crf1 parser consume: " << static_cast<double>(_timing_parser / 1000) << "ms" << std::endl;
#endif
}

void CRFProcessor::_process(TokenImpl *token, std::vector<TokenImpl *> &out)
{
	size_t length, i;
	const char *s;
	char tag;
#ifdef TIMING	
	struct timeval tv[2];
	struct timezone tz;
#endif

	_tagger->clear();
	s = token->get_token();
	length = token->get_length();
	if (length > _result_size) {
		_result_size = (length > (_result_size << 1))?length + 1:(_result_size << 1);
		_result = (char *)realloc(_result, _result_size);
	}
	for (i = 0; i < length; i++) {
		utf8::sub(_token, s, i, 1);
		_tagger->add(_token);
	}
#ifdef TIMING		
		gettimeofday(&tv[0], &tz);
#endif
	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");
#ifdef TIMING
		gettimeofday(&tv[1], &tz);
		_timing_parser += (tv[1].tv_sec - tv[0].tv_sec) * 1000000 + (tv[1].tv_usec - tv[0].tv_usec);
#endif

	_result_top = _result;
	for (i = 0; i < _tagger->size(); ++i) {
		s = _tagger->x(i, 0);
		while ( (*(_result_top++) = *(s++)) != 0)
			;
		*_result_top--;
		tag = *_tagger->y2(i);
		if (tag == 'S' || tag == 'E') {
			*(_result_top) = '\0';
			out.push_back(new TokenImpl(_result, TokenImpl::attr_cword));
			_result_top = _result;
		}
	}
}


} //namespace bamboo

