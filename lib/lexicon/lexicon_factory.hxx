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

#ifndef LEXICON_FACTORY_HXX
#define LEXICON_FACTORY_HXX

#include <cstring>
#include <exception>
#include <stdexcept>

#include "ilexicon.hxx"
#include "trie_lexicon.hxx"

namespace bamboo {


class LexiconFactory {
private:
	LexiconFactory();
public:
	static ILexicon *create(const char *type)
	{
		ILexicon *dc = NULL;
		if (type == NULL) return NULL;
		if (strcmp(type, "datrie") == 0) {
			dc = new TrieLexicon<DATrie>(1024);
		} else if (strcmp(type, "double_array") == 0) {
			dc = new TrieLexicon<DoubleArray>(1024);
		} else {
			throw std::runtime_error("unknow lexicon type " + std::string(type));
		}

		return dc;
	}

	static ILexicon *load(const char *filename)
	{
		FILE *fp = NULL;
		char magic[32];

		if (filename == NULL) return NULL;
		fp = fopen(filename, "r");
		if (fp == NULL) throw std::runtime_error("can not open lexicon: " + std::string(filename));
		fread(magic, sizeof(magic), 1, fp);
		fclose(fp);

		if (strcmp(magic, "datrie") == 0) {
			return new TrieLexicon<DATrie>(filename);
		} else if (strcmp(magic, "double_array") == 0) {
			return new TrieLexicon<DoubleArray>(filename);
		}
		return NULL;
	}
};

} //namespace bamboo

#endif // LEXICON_FACTORY_HXX
