#ifndef KEA_DOC_HXX
#define KEA_DOC_HXX

#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <set>

namespace bamboo { namespace kea {

class YCToken {
protected:
	char * _token;
	int _pos;
	int _tok_id;

public:
	YCToken(const char * s = NULL, int p = 0, int tok = 0)
		:_token(NULL),_pos(0),_tok_id(0)
	{
		if(s) _token = strdup(s);
		_pos = p;
		_tok_id = tok;
	}

	~YCToken() {
		if(_token) free(_token);
		_pos = 0;
		_tok_id = 0;
	}

	YCToken(const YCToken& t)
		:_token(NULL),_pos(0),_tok_id(0)
	{
		if(t._token) _token = strdup(t._token);
		_pos = t._pos;
		_tok_id = t._tok_id;
	}

	YCToken &operator=(const char *s) {
		set_token(s);
		return *this;
	}

	void set_token(const char *s) {
		if(_token) free(_token);
		if(s) _token = strdup(s);
		else _token = NULL;
	}

	void set_pos(int p) {
		_pos = p;
	}

	void set_id(int id) {
		_tok_id = id;
	}

	char * get_token() {
		return _token;
	}

	int get_pos() {
		return _pos;
	}

	int get_id() {
		return _tok_id;
	}

};

class YCSentence {
public:
	bool is_title;
	double rel_weight;
	std::vector<YCToken *> token_list;
	std::map<int, int> token_id_list;

	YCSentence():is_title(false),rel_weight(0) {}
	~YCSentence() {
		std::vector<YCToken *>::iterator yct_it
			= token_list.begin();
		for(; yct_it != token_list.end(); ++yct_it) {
			delete *yct_it;
		}
		token_list.clear();
	}
};

class YCDoc {
protected:
	struct YCStrCmp {
		bool operator() (const char * l, const char *r) {
			return strcmp(l, r) < 0;
		}
	};

public:
	typedef std::map<char *, int, YCStrCmp> oov_map;

	std::vector<YCSentence *> sent_list;
	std::map<int, char *> token_id_map;
	std::set<int> token_in_title;
	std::set<int> token_ner;
	oov_map oov_token;
	YCDoc() {}
	~YCDoc() {
		std::vector<YCSentence *>::iterator ycs_it
			= sent_list.begin();
		for(; ycs_it != sent_list.end(); ++ycs_it) {
			delete *ycs_it;
		}
		sent_list.clear();
	}
};

}} //end of namespace bamboo::kea

#endif //end of KEA_DOC_HXX
