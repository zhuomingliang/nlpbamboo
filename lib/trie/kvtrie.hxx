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

#ifndef KVTRIE_HXX
#define KVTRIE_HXX

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "mmap.hxx"
#include "datrie.hxx"

namespace bamboo {


class KVTrie {
protected:
	DATrie _index;
	MMap _data;

public:
	KVTrie(const char *index, const char *data)
		:_index(index), _data(data)
	{
	}

	static void build_from_text(const char *index, const char *data, 
			const char *source, bool verbose = false)
	{
		std::ifstream ifs;
		std::ofstream ofs;
		std::string s;
		int value_start;
		DATrie trie;

		if (verbose)
			std::cout << "building from " << source << std::endl;
		ifs.open(source);
		ofs.open(data);
		if (!ifs.is_open()) throw std::runtime_error("error on reading source");
		if (!ofs.is_open()) throw std::runtime_error("error on writting data");
		ofs << "\n";
		while(!ifs.eof()) {
			std::getline(ifs, s);
			if (s.empty()) continue;
			value_start = s.find(' ') + 1;
			trie.insert(s.substr(0, value_start - 1).c_str(), ofs.tellp());
			ofs << s.substr(value_start);
			ofs.write("", 1);
		}
		trie.save(index);
	}

	
	const char* operator[](const char * k)
	{
		int v = _index.search(k);
		if (v > 0)
			return (const char *)_data.start(_index.search(k));
		else
			return NULL;
	}
};

} //namespace bamboo

#endif // KVTRIE_HXX
