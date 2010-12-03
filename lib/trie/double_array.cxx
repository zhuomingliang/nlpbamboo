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

#include "double_array.hxx"

namespace bamboo {


DoubleArray::DoubleArray(int num)
	:_header(NULL), _state(NULL), _mmap(NULL)
{
	_header = new _header_t;
	_header->max = 0;
	_header->min = 0xffffff;
	_header->sum = 0;
	_header->num = 0;
	_header->num_insert = 0;
	strcpy(_header->magic, "double_array");
	_inflate(num);
	_last = 1;
}

DoubleArray::DoubleArray(const char *filename)
	:_header(NULL), _state(NULL), _mmap(NULL)
{
	_mmap = new MMap(filename);
	_header = (_header_t *)_mmap->start();
	_state = (_state_t *)_mmap->start(sizeof(_header_t));
}

int DoubleArray::_find_base(int *key, int min, int max)
{
	bool found;
	int i, *p;

	for (i = _last, found = false;!found;) {
		i++;
		if (i + _key2state(max) >= _header->num) 
			_inflate(_key2state(max));
		if (_check(i + _key2state(min)) <= 0 &&
			_check(i + _key2state(max)) <= 0) {
			found = true;
			for (p = key; *p > -1; p++) {
				if (_check(i + _key2state(*p)) > 0) {
					found = false;
					break;
				}
			}
		} else {
			//i += min;
		}
	}
	_last = i;
	return i;
}

int DoubleArray::_relocate(int stand, int s, int *key, int max, int min)
{
	int old, neo, i, *p;
	int ancestor[alphabet_size] = {0};

	assert(s > 0 && s < _header->num);
	old = _base(s);
	neo = _find_base(key, max, min);
	for (i = 0; key[i] > -1; i++) {
		int t = _key2state(key[i]);

		if (_check(old + t) != s) continue;
		_set_base(neo + t, _base(old + t));
		_set_check(neo + t, _check(old + t));
		_find_accepts(old + t, ancestor, NULL, NULL);
		for (p = ancestor; *p > -1; p++) 
			_set_check(_base(old + t) + _key2state(*p), neo + t);
		if (stand == old + t) stand = neo + t; 
		_set_base(old + t, 0);
		_set_check(old + t, 0);
		assert(_check(old + t) == 0 && _base(old + t) == 0);
	}	
	_set_base(s, neo);
	return stand;
}

int DoubleArray::_create_transition(int s, int ch)
{
	int t, n, m, ns[alphabet_size] = {0}, ms[alphabet_size] = {0};
	int max[] = {0, 0}, min[] = {0, 0};

	assert(s > 0 && s < _header->num);
	t = _next(s, ch);
	if (_base(s) > 0 && _check(t) <= 0) {
	} else {
		n = _find_accepts(s, ns, &max[0], &min[0]);
		m = (_check(t) == 0)?0:_find_accepts(_check(t), ms, &max[1], &min[1]);
		if (m > 0 && n + 1 > m) {
			s = _relocate(s, _check(t), ms, max[1], min[1]); 
		} else {
			ns[n++] = ch;
			ns[n] = -1;
			if (ch > max[0] || max[0] == 0) max[0] = ch;
			if (ch < min[0] || min[0] == 0) min[0] = ch;
			s = _relocate(s, s, ns, max[0], min[0]);
		}
		t = _next(s, ch);
		assert(_base(s) > 0 && _check(t) <= 0);
	}
	_set_check(t, s);

	return t;
}


void DoubleArray::insert(const char *key, int val)
{
	const char *p;
	int s, t;

	if (val < 0) throw std::runtime_error("Invalid value");
	if (key == NULL) throw std::runtime_error("Empty Key");
	for (p = key, s = 1;; p++) {
		t = _forward(s, (unsigned char)*p);
		if (t == 0) {
			for (;; p++) {
				s = _create_transition(s, (unsigned char)*p);
				if (*p == '\0') break;
			}
			break;
		}
		s = t; /* move to next state */
		if (*p == '\0') break;
	}
	_set_base(s, val);
	_update_header(val);
}

int DoubleArray::search(const char *key)
{
	const char *p;
	int s, t;
	
	for (p = key, s = 1;; *p++) {
		t = _forward(s, (unsigned char)*p);
		if (t == 0) return 0;
		s = t;
		if (*p == '\0') break;
	}
	return s?_base(s):0;
}

void DoubleArray::_explore(on_explore_finish_t cb, void *arg, int s, int off)
{
	int *p, key[alphabet_size];
	int t;

	assert(s > 0 && s < _header->num);
	if (_find_accepts(s, key, NULL, NULL) > 0) {
		for (p = key; *p > -1; p++) {
			_explore_buff[off] = (char)*p;
			t = _next(s, *p);
			assert(t < _header->num && t > 0);
			_explore(cb, arg, t, off + 1);
		}
	} else {
		_explore_finish(cb, arg, s, off);
	}
}

void DoubleArray::save(const char *filename)
{
	FILE *fp;

	assert(filename);
	fp = fopen(filename, "w+");
	if (fp == NULL)
		throw std::runtime_error("Can not write to file");
	fwrite(_header, sizeof(_header_t), 1, fp);
	fwrite(_state, _header->num * sizeof(_state_t), 1, fp);
	fclose(fp);
}


} //namespace bamboo

