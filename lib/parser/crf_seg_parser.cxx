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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "config_finder.hxx"
#include "crf_seg_parser.hxx"

namespace bamboo {


CRFSegParser::CRFSegParser(const char *file, bool verbose)
:_verbose(verbose), _config(NULL), _in(&_token_fifo[0]), _out(&_token_fifo[1])
{
	ConfigFinder		*finder;
	ProcessorFactory	*factory;

	finder = ConfigFinder::get_instance();
	_config = finder->find("crf_seg.conf", file, _verbose);
	(*_config)["prepare_characterize"] = "1";

	_config->get_value("verbose", _verbose);
	_config->get_value("use_break", _use_break);
	_config->get_value("use_single_combine", _use_single_combine);

	factory = ProcessorFactory::get_instance();
	factory->set_config(_config);

	_procs.push_back(factory->create("prepare"));
	_procs.push_back(factory->create("crf_seg"));
	if (_use_single_combine)
		_procs.push_back(factory->create("single_combine"));
	if (_use_break)
		_procs.push_back(factory->create("break"));
}

CRFSegParser::~CRFSegParser()
{
	size_t i;

	i = _procs.size();
	while(i--) delete _procs[i];
	_procs.clear();
	delete _config;
}

int
CRFSegParser::parse(std::vector<Token *> &out)
{
	size_t					i, length, space_cnt = 0;
	const char				*s;

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
	for (i = 0; i < length; i++) {
		if(*((*_in)[i]->get_orig_token()) == ' ') {
			delete (*_in)[i];
			++space_cnt;
			continue;
		}
		out.push_back((*_in)[i]);
	}

	return length - space_cnt;
}

} //namespace bamboo
