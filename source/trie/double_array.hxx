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

#ifndef DOUBLE_ARRAY_HXX
#define DOUBLE_ARRAY_HXX

#include <cstdlib>
#include <cassert>
#include <cstring>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <cstdio>

#include "mmap.hxx"

namespace bamboo {


typedef void (*on_explore_finish_t)(const char*, int, void *);
class DoubleArray {
	friend class TrieDebugger;

public:	
	static const int alphabet_size = 257;
	static const int endmark = 1;
	static const int magic_size = 32;

	DoubleArray(int num=_default_num_state);
	DoubleArray(const char *filename);
	virtual ~DoubleArray()
	{
		if (_mmap) {
			delete _mmap;
		} else {
			delete _header;
			free(_state);
		}
	}

	void explore(on_explore_finish_t cb, void *arg)
	{
		_explore(cb, arg, 1, 0);
	}

	int max_value()
	{
		return _header->max;
	}

	int min_value()
	{
		return _header->min;
	}

	int sum_value()
	{
		return _header->sum;
	}

	int num_insert()
	{
		return _header->num_insert;
	}

	void insert(const char *key, int val);
	int search(const char *key);
	void save(const char *filename);

protected:
	static const int _default_num_state = 1024;
	static const int _explore_buff_size = 1024;

	#pragma pack(push, 4)
	typedef struct {
		char magic[magic_size];
		int num;
		int max, min;
		long long sum;
		int num_insert;
	}  __attribute__((aligned(4))) _header_t;

	typedef struct {
		int base;
		int check;
	} _state_t;
	#pragma pack(pop)

	int _last;

	_header_t *_header;
	_state_t *_state;
	MMap *_mmap;
	char _explore_buff[_explore_buff_size];

	int _key2state(int ch)
	{
		return (unsigned int)ch + 1;
	}

	int _state2key(int s)
	{
		assert(s > 0);
		return s - 1;
	}

	int _base(int s)
	{
		assert(s > 0 && s < _header->num);
		return _state[s].base;
	}

	int _check(int s)
	{
		assert(s > 0 && s < _header->num);
		return _state[s].check;
	}

	void _set_base(int s, int val)
	{
		assert(s > 0 && s < _header->num);
		_state[s].base = val;
	}

	void _set_check(int s, int val)
	{
		assert(s > 0 && s < _header->num);
		_state[s].check = val;
	}

	int _next(int s, int ch)
	{
		int in = _key2state(ch);

		assert(s > 0 && s < _header->num); 
		if (s + in >= _header->num) _inflate(in);
		return _base(s) + in;
	}

	int _forward(int s, int ch)
	{
		assert(s > 0 && s < _header->num); 
		int t = _next(s, ch);
		return (t > 0 && t < _header->num && _check(t) == s)?t:0;
	}

	void _inflate(int num)
	{
		int neo;

		assert(num > 0);
		neo = (((_header->num + num) >> 12) + 1) << 12; // align for 4096
		_state = (_state_t *)realloc(_state, neo * sizeof(_state_t));
		memset(_state + _header->num, 0, (neo - _header->num) * sizeof(_state_t));
		if (_state == NULL) throw std::bad_alloc();
		_header->num = neo;
	}

	int _find_accepts(int s, int *inputs, int *max, int *min)
	{
		int ch;
		int *p;

		assert(s > -1 && s < _header->num);
		assert(inputs);
		for (ch = 0, p = inputs; ch < alphabet_size; ch++) {
			if (_forward(s, ch)) {
				*(p++) = ch;
				if (max && (ch > *max || *max == 0)) *max = ch;
				if (min && (ch < *min || *min == 0)) *min = ch;
			}
		}
		*p = -1;
		return p - inputs;
	}

	void _update_header(int val)
	{
		_header->max = (val > _header->max)?val:_header->max;
		_header->min = (val < _header->min)?val:_header->min;
		_header->sum += val;
		_header->num_insert++;
	}

	virtual void _explore_finish(on_explore_finish_t cb, void *arg, int s, int off) 
	{
		cb(_explore_buff, _base(s), arg);
	}

	int _find_base(int *key, int max, int min);
	int _relocate(int stand, int s, int *key, int max, int min);
	int _create_transition(int s, int ch);
	void _explore(on_explore_finish_t cb, void *arg, int s, int off);
private:
	DoubleArray(DoubleArray &) {}

};

} //namespace bamboo

#endif // DOUBLE_ARRAY_HPP
