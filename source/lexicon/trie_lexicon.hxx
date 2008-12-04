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

#ifndef TRIE_LEXICON_HXX
#define TRIE_LEXICON_HXX

#include <cstdio>

#include "datrie.hxx"

namespace bamboo {


template<class TrieType>
class TrieLexicon:public ILexicon {
	friend class TrieDebugger;
protected:
	TrieLexicon();
public:
	TrieType *_trie;
	TrieLexicon(int size)
	{
		_trie = new TrieType(size);
	}
	
	TrieLexicon(const char *filename)
	{
		assert(filename);
		_trie = new TrieType(filename);
	}
	
	~TrieLexicon()
	{
		delete _trie;
	}
	
	void insert(const char *s, int val)
	{
		_trie->insert(s, val);
	}

	int search(const char *s)
	{
		return _trie->search(s);
	}
	
	int operator[](const char *s)
	{
		return search(s);
	}

	void save(const char *filename) 
	{
		assert(filename);
		_trie->save(filename);
	}	

	void explore(on_explore_finish_t cb, void *arg) {
		_trie->explore(cb, arg);
	}

	void read_from_text(const char *filename, bool verbose)
	{
		FILE *fp;
		char str[4096];
		int val;
		size_t i;

		assert(filename);
		fp = fopen(filename, "r");
		if (verbose)
			std::clog << "making index" << std::endl;
		for(i = 0; ; i++) {
			if (fscanf(fp, "%d %4096[^\r\n]", &val, str) == EOF) break;
			insert(str, val);
			if (verbose && i % 500 == 0) 
				std::clog << "\r\t\t" << i << " items processed.";
		}
		fclose(fp);
		if (verbose)
			std::clog << "\r\t\t" << i << " items processed." << std::endl;
	}

	void write_to_text(const char *filename)
	{
		FILE *fp;

		assert(filename);
		fp = fopen(filename, "w+");
		explore(_export, fp);		
		fclose(fp);
	}

	int max_value()
	{
		return _trie->max_value();
	}

	int min_value()
	{
		return _trie->min_value();
	}

	int sum_value()
	{
		return _trie->sum_value();
	}

	int num_insert()
	{
		return _trie->num_insert();
	}

	//void read_from_text(const char *filename, bool verbose=false);
	//void write_to_text(const char *filename);
};

} //namespace bamboo

#endif // TRIE_LEXICON_HXX
