#ifndef LEX_TOKEN_CXX
#define LEX_TOKEN_CXX

#include <cstdlib>
#include <cstring>

class LexToken {
protected:
	char *_token;
	int _attr;
public:
	enum attr_t {
		attr_unknow = 0,
		attr_number,
		attr_alpha,
		attr_cword,
		attr_punct,
	};
	LexToken():_token(NULL), _attr(attr_unknow) {}
	LexToken(const char *s, int attr = attr_unknow)
		:_attr(attr)
	{
		_token = strdup(s);
	}
	~LexToken()
	{
		if (_token) {
			free(_token);
			_token = NULL;
		}
	}
	int get_attr() {return _attr;}
	char *get_token() {return _token;}
};

#endif

