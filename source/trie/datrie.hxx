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

#ifndef DATRIE_HXX
#define DATRIE_HXX

#include "double_array.hxx"

namespace bamboo {


class DATrie: public DoubleArray {
	friend class TrieDebugger;
private:
	DATrie(DATrie &trie) {}

protected:
	#pragma pack(push, 4)
	typedef struct {
		int num;
		int last;
	} _extra_t;

	typedef struct {
		int *s;
		int length;
	} _suffix_t;
	#pragma pack(pop)

	_extra_t *_extra;
	int *_tail;
	_suffix_t _suffix;

	void _inflate_tail(int num)
	{
		int neo;

		assert(num > 0);
		neo = (((_extra->num + num) >> 12) + 1) << 12; // align for 32 bits
		_tail = (int *)realloc(_tail, neo * sizeof(int));
		memset(_tail + _extra->num, 0, (neo - _extra->num) * sizeof(int));
		if (_tail == NULL) throw std::bad_alloc();
		_extra->num = neo;
	}

	virtual void _explore_finish(on_explore_finish_t cb, void *arg, int s, int off) 
	{
		int p, val;

		if (_explore_buff[off - 1] == '\0') {
			if (_base(s) < 0) {
				val = *(_tail - _base(s));
			} else {
				val = _base(s);
			}
		} else {	
			for (p = -_base(s); _tail[p]; p++) 
				_explore_buff[off++] = _tail[p];
			_explore_buff[off] = '\0';
			val = *(_tail + p + 1);
		}
		cb(_explore_buff, val, arg);
	}

	void _insert_tail(int s, const char *key, int val);
	void _branch(int s, const char *key, int val);

public:
	DATrie(int size=_default_num_state)
		: DoubleArray(size), _extra(NULL), _tail(NULL)
	{
		_extra = new _extra_t;
		_extra->num = 0;
		_extra->last = 1;
		_suffix.s = NULL;
		_suffix.length = 0;
		_inflate_tail(size);
		strcpy(_header->magic, "datrie");
	}

	DATrie(const char *filename)
		: DoubleArray(filename)
	{
		int start = sizeof(_header_t) + _header->num * sizeof(_state_t);
		_extra = (_extra_t *)_mmap->start(start);
		_tail = (int *)_mmap->start(start + sizeof(_extra_t));
		_suffix.s = NULL;
		_suffix.length = 0;
	}

	~DATrie()
	{
		if (!_mmap) {
			delete _extra;
			free(_tail);
			free(_suffix.s);
			_suffix.s = 0;
		}
	}
	void insert(const char *key, int val);
	int search(const char *key);
	void save(const char *filename);
};

} //namespace bamboo

#endif // DATRIE_HPP
