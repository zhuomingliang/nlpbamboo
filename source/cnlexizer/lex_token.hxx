#ifndef LEX_TOKEN_CXX
#define LEX_TOKEN_CXX

#include <cstdlib>
#include <cstring>

#include "utf8.hxx"

class LexToken {
protected:
	char *_orig_token;
	char *_token;
	int _attr;
	size_t _length;
	size_t _bytes;
public:
	enum attr_t {
		attr_unknow = 0,
		attr_number,
		attr_alpha,
		attr_cword,
		attr_punct,
	};
	LexToken():_token(NULL), _orig_token(NULL), _attr(attr_unknow), _length(0), _bytes(0) {}
	LexToken(const char *s, const char *os, int attr = attr_unknow)
		:_orig_token(NULL), _attr(attr), _length(0), _bytes(0)
	{
		_token = strdup(s);
		_orig_token = strdup(os);
		_length = utf8::length(s);
		_bytes = strlen(s);
	}
	LexToken(const char *s, int attr = attr_unknow)
		:_orig_token(NULL), _attr(attr), _length(0), _bytes(0)
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
	int set_attr(int attr) {
		_attr = attr;
	}
	size_t get_length() {return _length;}
	size_t get_bytes() {return _bytes;}
};

#endif

