#ifndef YCAKE_TOKEN_DICT_HXX
#define YCAKE_TOKEN_DICT_HXX

#include "lexicon_factory.hxx"
#include "ilexicon.hxx"
#include "datrie.hxx"
#include <cmath>

namespace bamboo { namespace ycake {

class TokenDict {
protected:
	int _D;
	int _max_id;

	bool _is_init;

	bamboo::ILexicon * _token_id;
	bamboo::ILexicon * _token_df;

	int _df_avg;

protected:
	int _get_df(const char * token) {
		int df = 0;
		if(_token_df) {
			df = _token_df->search(token);
		}
		if(df <= 0) {
			df = 0;
		}
		return df;
	}

	int _get_total_docs() {
		if(_D==0 && _token_df) {
			_D = _token_df->max_value() + 1;
		}
		return _D;
	}

public:
	TokenDict():_D(0),_max_id(0),_is_init(false),_token_id(NULL),_token_df(NULL),_df_avg(0) {}
	~TokenDict() {
		if(_token_id)
			delete _token_id;
		if(_token_df)
			delete _token_df;
	}

	int init(const char * token_id_dict, const char * token_df_dict) {
		_token_id = LexiconFactory::load(token_id_dict);
		_token_df = LexiconFactory::load(token_df_dict);
		_df_avg = _token_df->sum_value() / _token_df->num_insert();
		_is_init = true;
		return 0;
	}

	bool is_init() {
		return _is_init;
	}

	static TokenDict & get_instance() {
		static TokenDict dict;
		return dict;
	}

	int operator[] (const char * token) {
		return get_id(token);
	}

	//id is no less than 1, if token exists in the dict
	int get_id(const char * token) {
		int id = 0;
		if(_token_id) {
			id = _token_id->search(token);
			if(id <= 0) id = 0;
		}
		return id;
	}

	int get_max_id() {
		if(_max_id==0 && _token_id) {
			_max_id = _token_id->max_value();
		}
		return _max_id;
	}

	double get_idf(const char * token) {
		int df = _get_df(token);
		if(df == 0) {
			df = _df_avg;
		}
		float idf = 1;
		int d = _get_total_docs();
		if(df>0 && d>0) {
			idf = 1 + log(d/df); 
		}
		return idf;
	}
};

}} //end of namespace bamboo::ycake

#endif //end of YCAKE_TOKEN_DICT_HXX
