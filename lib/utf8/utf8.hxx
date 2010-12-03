/*
 * Copyright (c) 2008, detrox@gmail.com, weibingzheng@gmail.com
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
 * THIS SOFTWARE IS PROVIDED BY {detrox,weibingzheng}@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL {detrox,weibingzheng}@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef UTF8_XXX
#define UTF8_XXX
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <iostream>

namespace bamboo {


class utf8 {
protected:
	static const char _map[], _map_ignore[];
public:
	static size_t first(const char *s, char *utf8ch)
	{
		register size_t i;
		i = _map_ignore[(unsigned char)*s];
		memcpy(utf8ch, s, i);
		utf8ch[i] = '\0';
		return i;
	}

	static int locate(const char *s, size_t start)
	{
		size_t i, j;

		if (s == NULL) return 0; 
		for (i = 0, j = 0; j < start && s[i];) {
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
				j++;
			} else { 
				++i;
			}
		}
		return i;
	}

	static int check(const char *s)
	{
		size_t i;

		if (s == NULL) return 0; 
		for (i = 0; s[i];) {
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
			} else { 
				return -1;
			}
		}
		return 0;
	}

	static int strstr(const char *haystack, const char *needle)
	{
		size_t i, step, length;
		
		length = strlen(needle);
		if (haystack == NULL || needle == NULL || length == 0) return -1; 
		for (i = 0; haystack[i];) {
			step = (_map[(unsigned char)haystack[i]] > 0)?_map[(unsigned char)haystack[i]]:1;
			if (length == step && strncmp(haystack + i, needle, length) == 0) return i;
			i += step;
		}
		return -1;
	}

	static int index(const char *s, size_t *index)
	{
		size_t i, j;

		if (s == NULL) return 0; 
		for (i = 0, j = 0; s[i];) {
			index[j++] = i;
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
			} else { 
				++i;
				continue;
			}
		}
		return j;
	}

	static size_t length(const char *s)
	{
		size_t i, j;

		if (s == NULL) return 0; 
		for (i = 0, j = 0; s[i];) {
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
				++j;
			} else { 
				++i;
				continue;
			}
		}
		return j;
	}

	static size_t sub(char *t, const char *s, size_t start, size_t length)
	{
		register size_t i;
		register char *begin = t;

		for (i = 0; *s && i < start; s += _map_ignore[(unsigned char)*s], ++i) ;
		while(length--) {
			i = _map_ignore[(unsigned char)*s];
			while(i--) *(t++) = *(s++);
		}
		*t = '\0';
		return t - begin;
	}

	static char dbc2sbc(const char *str, size_t len) 
	{
		/* Reference: http://zh.wikipedia.org/wiki/%E5%85%A8%E8%A7%92 */
		register unsigned char uch1, uch2, uch3, sbc;

		if ((unsigned char)*str < 128) return *str;
		if (len != 3) return 0;

		uch1 = str[0];
		uch2 = str[1];
		uch3 = str[2];

		if(uch1 == 0xE3 && uch2 == 0x80 && uch3 == 0x80) {
			sbc = 0x20;
		} else if(uch1 == 0xEF && uch2 == 0xBC && uch3 >= 0x81 && uch3 <= 0xBF) {
			sbc = uch3 - 0x60;
		} else if(uch1 == 0xEF && uch2 == 0xBD && uch3 >= 0x80 && uch3 <= 0x9E) {
			sbc = uch3 - 0x20;
		} else {
			sbc = 0x0;
		}

		return sbc;
	}
};

} //namespace bamboo

#endif
