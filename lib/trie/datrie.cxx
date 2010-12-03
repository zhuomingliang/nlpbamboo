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

#include "datrie.hxx"

namespace bamboo {


void DATrie::_insert_tail(int s, const char *key, int val)
{
	const char *p;

	assert(s > 0 && s < _header->num); assert(key);
	_set_base(s, -(_extra->last));
	
	for (p = key;*p; p++) {
		if (_extra->last + 32 >= _extra->num) _inflate_tail(32);
		assert(_tail[_extra->last] == 0);
		_tail[_extra->last++] = (unsigned char)*p;
	}
	_tail[_extra->last++] = 0;
	_tail[_extra->last++] = val;
	_update_header(val);
}

void DATrie::_branch(int s, const char *suffix, int val)
{
	int i, j, t, *key, start, base, max, min;

	assert(suffix); assert(s > 0 && s < _header->num);
	assert(_base(s) < 0);
	key = _suffix.s;
	start = -(_base(s));
	for (i = 0, max = 0, min = 0;; i++) {
		/* expand by 4k */
		if (i + 2 >= _suffix.length) {
			/* +2 for trailing marks*/
			int neo = (((_suffix.length + i + 2) >> 8) + 1) << 8;
			_suffix.s = (int *)realloc(_suffix.s, neo * sizeof(int));
			_suffix.length = neo;
			key = _suffix.s;
		}
		key[i] = (unsigned char)suffix[i];
		if (key[i] > max || max == 0) max = key[i];
		if (key[i] < min || min == 0) min = key[i];
		if (*(_tail + start + i) != key[i] || key[i] == 0) break;
	}
	key[i + 1] = -1;
	if (*(_tail + start + i)  == key[i]) {
		*(_tail + start + i + 1) = val;
		return;
	}
	base = _find_base(key, max, min);
	_set_base(s, base);
	for (j = 0, t = s; j < i; j++) {
		t = _create_transition(t, key[j]);
	}
	// create twig
	s = _create_transition(t, *(_tail + start + i));
	_set_base(s, -(start + i + 1));
	s = _create_transition(t, key[i]);
	if (key[i] == 0) {
		_set_base(s, val);
		_update_header(val);
	} else {
		_insert_tail(s, suffix + i + 1, val);
	}
}

void DATrie::insert(const char *key, int val)
{
	int i, s, t;
	const char *p;

	for (p = key, i = 0, s = 1;; p++, i++) {
		if (_base(s) < 0) {
			_branch(s, p, val);
			return;
		}
		t = _forward(s, (unsigned char)*p);
		if (t == 0) {
			t = _create_transition(s, (unsigned char)*p);
			if (*p == 0) {
				_set_base(t, val);
				_update_header(val);
			} else {
				_insert_tail(t, p + 1, val);
			}
			return;
		}
		s = t;
		if (*p == '\0') break;
	}
	_set_base(s, val);
}

int DATrie::search(const char *key)
{
	int s, t, *q;
	const char *p;

	for (s = 1, p = key;; p++) {
		if (_base(s) < 0) {
			for (q = _tail - _base(s);; q++, p++) {
				if (*q != (unsigned char)*p) return 0;
				if (*q == 0) break;
			}
			return *(q + 1);
		}
		t = _forward(s, (unsigned char)*p);
		if (t == 0) return 0;
		s = t;
		if (*p == 0) break;
	}
	t = _base(s);
	return (t < 0)?*(_tail - t):_base(s);
}

void DATrie::save(const char *filename)
{
	FILE *fp = NULL;

	assert(filename);
	fp = fopen(filename, "w+");

	if (fp == NULL)
		throw std::runtime_error("Can not write to file");

	fwrite(_header, sizeof(_header_t), 1, fp);
	fwrite(_state, _header->num * sizeof(_state_t), 1, fp);

	fwrite(_extra, sizeof(_extra_t), 1, fp);
	fwrite(_tail, _extra->num * sizeof(int), 1, fp);
	fclose(fp);
}


} //namespace bamboo

