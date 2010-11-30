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

#include "config_finder.hxx"
#include "keyword_parser.hxx"

namespace bamboo {

KeywordParser::KeywordParser(const char *file, bool verbose)
:_verbose(verbose), _config(NULL), _ke(NULL)
{
	ConfigFinder * finder;

	finder = ConfigFinder::get_instance();

	_config = finder->find("keyword.conf", file, _verbose);
	_config->get_value("verbose", _verbose);

	_ke = new bamboo::kea::KeywordExtractor(_config);
}

KeywordParser::~KeywordParser() {
	if(_ke) {
		delete _ke;
	}
}

int KeywordParser::parse(std::vector<Token *> &out) {
	size_t i, len;
	const char *title, *text;

	title = (const char *)getopt(BAMBOO_OPTION_TITLE);
	text = (const char *)getopt(BAMBOO_OPTION_TEXT);

	std::vector<std::string> res;
	res.reserve(_ke->max_keywords());
	_ke->get_keyword(title, text, res);
	len = res.size();
	for(i=0; i<len; ++i) {
		out.push_back(new TokenImpl(res[i].c_str()));
	}
	return len;
}

} //namespace bamboo
