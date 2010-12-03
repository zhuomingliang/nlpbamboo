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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <map>

#include "config_finder.hxx"
#include "crf_ner_ns_parser.hxx"

namespace bamboo {


CRFNSParser::CRFNSParser(const char *file, bool verbose)
:_verbose(verbose), _config(NULL), _in(&_token_fifo[0]), _out(&_token_fifo[1]), _output_type(0)
{
	ConfigFinder * finder;
	ProcessorFactory * factory;

	finder = ConfigFinder::get_instance();
	_config = finder->find("crf_ner_ns.conf", file, _verbose);
	(*_config)["prepare_characterize"] = "1";

	_config->get_value("verbose", _verbose);

	factory = ProcessorFactory::get_instance();
	factory->set_config(_config);

	_procs.push_back(factory->create("prepare"));
	_procs.push_back(factory->create("crf_seg4ner"));
	_procs.push_back(factory->create("crf_ner_ns"));

	_config->get_value("ner_output_type", _output_type);
}

CRFNSParser::~CRFNSParser()
{
	size_t i;
	i = _procs.size();
	while(i--) delete _procs[i];
	_procs.clear();
	delete _config;
}

int
CRFNSParser::parse(std::vector<Token *> &out)
{
	size_t i, length, space_cnt = 0;
	const char *s;

	s = (const char *)getopt(BAMBOO_OPTION_TEXT);

	length = utf8::length(s);
	_in->clear();
	if (length > _in->capacity()) {
		_in->reserve(length << 1);
		_out->reserve(length << 1);
	}
	_in->push_back(new TokenImpl(s));
	length = _procs.size();
	for (i = 0; i < length; i++) {
		_out->clear();
		_procs[i]->process(*_in, *_out);
		/* switch in & out queue */
		_swap = _out;
		_out = _in;
		_in = _swap;
	}

	length = _in->size();
	std::map<std::string, Token*> uniq_token;
	std::map<std::string, Token*>::iterator uit;

	for (i = 0; i < length; i++) {
		s = (*_in)[i]->get_orig_token();
		if(*s == ' ') {
			delete (*_in)[i];
			++space_cnt;
			continue;
		}
		if(_output_type == 0) {
			uit = uniq_token.lower_bound(s);
			if(uit == uniq_token.end() || uit->first.compare(s)) {
				uniq_token.insert(std::make_pair(s, (*_in)[i]));
				out.push_back((*_in)[i]);
			} else {
				delete (*_in)[i];
			}
		} else {
			out.push_back((*_in)[i]);
		}
	}

	length -= space_cnt;
	if(_output_type == 0) length = uniq_token.size();

	return length;
}

} //namespace bamboo
