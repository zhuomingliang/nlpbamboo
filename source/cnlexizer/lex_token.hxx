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

#ifndef LEX_TOKEN_CXX
#define LEX_TOKEN_CXX

#include <cstdlib>
#include <cstring>

#include "utf8.hxx"

class LexToken {
protected:
	char *_orig_token;
	int _attr;
	size_t _length, _orig_length;
	size_t _bytes, _orig_bytes;
	char *_token;
	unsigned short _pos;
public:
	enum attr_t {
		attr_unknow = 0,
		attr_number,
		attr_alpha,
		attr_cword,
		attr_punct,
	};
	LexToken()
		:_orig_token(NULL), _attr(attr_unknow), _length(0), _orig_length(0), _bytes(0), _orig_bytes(0), _pos(0)
	{
	}
	LexToken(const char *s, const char *os, int attr = attr_unknow)
		:_orig_token(NULL), _attr(attr), _length(0), _orig_length(0), _bytes(0), _orig_bytes(0), _pos(0)
	{
		_token = strdup(s);
		_orig_token = strdup(os);
		_length = utf8::length(s);
		_bytes = strlen(s);
		_orig_length = utf8::length(os);
		_orig_bytes = strlen(os);
	}
	LexToken(const char *s, int attr = attr_unknow)
		:_orig_token(NULL), _attr(attr), _length(0), _orig_length(0), _bytes(0), _orig_bytes(0), _pos(0)
	{
		_token = strdup(s);
		_length = utf8::length(s);
		_bytes = strlen(s);
	}
	~LexToken()
	{
		if (_token) {
			free(_token);
			_token = NULL;
		}
		if(_orig_token) {
			free(_orig_token);
			_orig_token = NULL;
		}
	}
	int get_attr() {return _attr;}
	char *get_token() {return _token;}
	char *get_orig_token() {
		if(_orig_token) return _orig_token;
		return _token;
	}
	void set_token(const char *s, int attr) {
		set_token(s);
		set_attr(attr);
	}
	void set_token(const char *s) {
		if(!_orig_token) {
			_orig_token = _token;
		} else {
			free(_token);
		}
		_token = strdup(s);
		_length = utf8::length(s);
		_bytes = strlen(s);
	}
	void set_attr(int attr) {
		_attr = attr;
	}
	void set_pos(unsigned short pos) {
		_pos = pos;
	}
	void set_pos(const char *s) {
		_pos = *s;
		if(*(s+1)!=0) {
			_pos = _pos*256 + *(s+1);
		}
	}
	size_t get_length() {return _length;}
	size_t get_bytes() {return _bytes;}
	size_t get_orig_length() {return (_orig_length > 0)?_orig_length:_length;}
	size_t get_orig_bytes() {return (_orig_bytes > 0)?_orig_bytes:_bytes;}
	unsigned short get_pos() {return _pos;}
	
	static char * get_pos_str(unsigned short p) {
		char * pos_str = (char *)malloc(3);
		if(p>256) {
			sprintf(pos_str,"%c%c", p/256, p%256);
		} else {
			sprintf(pos_str,"%c", p%256);
		}
		return pos_str;
	}
};

#endif

