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

#ifndef PARSER_FACTORY_HXX
#define PARSER_FACTORY_HXX

#include <dlfcn.h>

#include <string>
#include <cassert>


#include "parser.hxx"
#include "crf_ner_nr_parser.hxx"
#include "crf_ner_ns_parser.hxx"
#include "crf_ner_nt_parser.hxx"
#include "crf_seg_parser.hxx"
#include "custom_parser.hxx"
#include "keyword_parser.hxx"
#include "ugm_seg_parser.hxx"

namespace bamboo {

class ParserFactory
{
private:

	static ParserFactory		*_instance;

protected:
	IConfig 					*_config;
	
	ParserFactory():_config(NULL) {}
	ParserFactory(const ParserFactory&):_config(NULL) {}
	ParserFactory& operator= (const ParserFactory &) { return *this;}
public:

	static ParserFactory *get_instance()
	{
		if (_instance == NULL)
			_instance = new ParserFactory();

		return _instance;
	}

	Parser *create(	const char *name,
					const char *cfg = NULL, bool verbose=false)
	{
#define register_parser(N, C) if (strcmp(name, (N)) == 0) return new (C)(cfg);
		register_parser("ugm_seg", UGMSegParser);
		register_parser("mfm_seg", UGMSegParser);
		register_parser("crf_seg", CRFSegParser);
		register_parser("crf_ner_nr", CRFNRParser);
		register_parser("crf_ner_ns", CRFNSParser);
		register_parser("crf_ner_nt", CRFNTParser);
		register_parser("keyword", KeywordParser);
#undef register_parser
		return NULL;
	}
};

};

#endif /* PARSER_FACTORY_HXX */
