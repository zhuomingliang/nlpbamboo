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

#include "custom_parser.hxx"

namespace bamboo {


CustomParser::CustomParser(const char *file, bool verbose)
:_verbose(0), _config(NULL), _in(&_token_fifo[0]), _out(&_token_fifo[1])
{
	_lazy_create_config(file);
	_init();
#ifdef TIMING
	memset(_timing_process, 0, sizeof(size_t) * 128);
#endif
}

CustomParser::~CustomParser()
{
	_fini();
	delete _config;
#ifdef TIMING
	size_t i;

	i = _processors.size();
	while(i--)
		std::cerr << "processor" << i << " consume: " << static_cast<double>(_timing_process[i] / 1000)<< "ms" << std::endl;
#endif
}

void CustomParser::_fini()
{
	size_t i;

	i = _processors.size();
	while(i--) delete _processors[i];
	_processors.clear();
}

void CustomParser::_init()
{
	std::vector<std::string>::iterator it;
	ProcessorFactory *factory;

	_config->get_value("verbose", _verbose);
	_config->get_value("process_chain", _process_chain);

	factory = ProcessorFactory::get_instance();
	factory->set_config(_config);

	for (it = _process_chain.begin(); it != _process_chain.end(); it++) {
		Processor *processor = NULL;
		processor = factory->create(it->c_str(),_verbose);
		_processors.push_back(processor);
	}
}

void CustomParser::_lazy_create_config(const char *custom)
{
	std::vector<std::string>::size_type i;
	bool flag = false;
	struct stat buf;
	std::vector<std::string> v;

	if (custom)
		v.push_back(custom);
	v.push_back("bamboo.cfg");
	v.push_back("etc/bamboo.cfg");
	v.push_back("/opt/bamboo/etc/bamboo.cfg");
	v.push_back("/etc/bamboo.cfg");
	for (i = 0; i < v.size(); i++) {
		if (_verbose)
			std::cerr << "loading configuration " << v[i] << " ... ";
		if (stat(v[i].c_str(), &buf) == 0) {
			if (_verbose) std::cerr << "found." << std::endl;
			_config = ConfigFactory::create(v[i].c_str());
			flag = true;
			break;
		} else {
			if (_verbose) std::cerr << "not found." << std::endl;
		}
	}

	if (!flag)
		throw std::runtime_error("can not find configuration");
}

int
CustomParser::parse(std::vector<Token *> &out)
{
#ifdef TIMING	
	struct timeval tv[2];
	struct timezone tz;
#endif
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
	length = _processors.size();
	for (i = 0; i < length; i++) {
		_out->clear();
#ifdef TIMING		
		gettimeofday(&tv[0], &tz);
#endif
		_processors[i]->process(*_in, *_out);
#ifdef TIMING
		gettimeofday(&tv[1], &tz);
		_timing_process[i] += (tv[1].tv_sec - tv[0].tv_sec) * 1000000 + (tv[1].tv_usec - tv[0].tv_usec);
#endif
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

void CustomParser::set(std::string s) 
{
	(*_config) << s;
}

void CustomParser::set(std::string key, std::string val) 
{
	(*_config)[key] = val;
}

void CustomParser::reload() 
{
	_fini();
	_init();
}

} //namespace bamboo
