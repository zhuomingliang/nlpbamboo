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

#ifndef TOKEN_IMPL_HXX
#define TOKEN_IMPL_HXX

#include <cstdlib>
#include <cstring>

#include "token.hxx"
#include "utf8.hxx"

namespace bamboo {

class TokenImpl:public Token {
protected:
	char *_orig_token, *_token;
	int _attr;
	size_t _length, _orig_length;
	size_t _bytes, _orig_bytes;
	unsigned short _pos;
	size_t refcount;
public:
	enum attr_t {
		attr_unknow = 0,
		attr_number,
		attr_alpha,
		attr_cword,
		attr_punct,
        attr_whitespace,
	};
	TokenImpl()
		:_orig_token(NULL), _token(NULL), _attr(attr_unknow), _length(0), 
		_orig_length(0), _bytes(0), _orig_bytes(0), _pos(0), refcount(0)
	{
	}
	TokenImpl(const char *s, const char *os, int attr = attr_unknow)
		:_orig_token(NULL), _token(NULL), _attr(attr), _length(0), 
		_orig_length(0), _bytes(0), _orig_bytes(0), _pos(0), refcount(0)
	{
		set_token(s);
		set_orig_token(os);
	}
	TokenImpl(const char *s, int attr = attr_unknow)
		:_orig_token(NULL),_token(NULL),  _attr(attr), _length(0), 
		_orig_length(0), _bytes(0), _orig_bytes(0), _pos(0), refcount(0)
	{
		set_token(s);
	}
	TokenImpl(const TokenImpl &rhs)
		:_orig_token(NULL), _token(NULL)
	{
		if (rhs._token) set_token(rhs._token);
		if (rhs._orig_token) set_orig_token(rhs._orig_token);
		_attr = rhs._attr;
		_pos = rhs._pos;
		_length = rhs._length;
		_orig_length = rhs._orig_length;
		_bytes = rhs._bytes;
		_orig_bytes = rhs._orig_bytes;
	}

	~TokenImpl()
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
	int get_attr() const 
	{
		return _attr;
	}
	const char *get_token() const 
	{
		return _token;
	}
	void set_token(const char *s)
	{
		assert(s);
		
		if (_token) 
			free(_token);
		_token = strdup(s);
		if (!_token)
			throw std::bad_alloc();
	}
	const char *get_orig_token() const
	{
		return (_orig_token)?_orig_token:_token;
	}
	void set_orig_token(const char *s)
	{
		assert(s);

		if (_orig_token) 
			free(_orig_token);
		_orig_token = strdup(s);
		if (!_orig_token)
			throw std::bad_alloc();
	}
	void set_attr(int attr) 
	{
		_attr = attr;
	}
	void set_pos(unsigned short pos) 
	{
		_pos = pos;
	}
	void set_pos(const char *s) 
	{
		assert(s);

		_pos = *s;
		if(*(s + 1)) 
			_pos = _pos * 256 + *(s + 1);
	}
	size_t get_length()
	{
		if (_length == 0) 
			_length = utf8::length(_token);
		return _length;
	}
	size_t get_bytes() 
	{
		if (_bytes == 0)
			_bytes = strlen(_token);
		return _bytes;
	}
	size_t get_orig_length() 
	{
		if (_orig_token) {
			if (_orig_length == 0) {
				_orig_length = utf8::length(_orig_token);
            }
            return _orig_length;
		} else {
			return get_length();
		}
	}
	size_t get_orig_bytes() 
	{
		if (_orig_token) {
			if (_orig_bytes == 0) {
				_orig_bytes = strlen(_orig_token);
            }
            return _orig_bytes;
		} else {
			return get_bytes();
		}
	}
	unsigned short get_pos() const 
	{
		return _pos;
	}
	size_t incref()
	{
		return ++refcount;
	}
	size_t decref()
	{
		if (refcount)
			--refcount;
		return refcount;
	}
};

} //namespace bamboo

#endif
