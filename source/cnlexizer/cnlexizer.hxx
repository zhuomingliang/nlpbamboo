#ifndef SEGMENTOR_HXX
#define SEGMENTOR_HXX

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cmath>

#include "lexicon_factory.hxx"
#include "config_factory.hxx"

class CNLexizer {
private:

public:
	size_t maxforward(char *t, const char *s);
	size_t maxbackward(char *t, const char *s);
	size_t unigram(char *t, const char *s);
	size_t bigram(char *t, const char *s);
	double ugm_score(const char *);

	CNLexizer(const char *file)
		:_config(NULL)
	{
		const char *s;
		_config = ConfigFactory::create("lua_config", file);
		_config->get_value("lexicon_unigram", s);
		_lexicon.ugm = LexiconFactory::load(s);
		_config->get_value("lexicon_unigram_sgt", s);
		_lexicon.ugm_sgt = LexiconFactory::load(s);
		_config->get_value("lexicon_bigram", s);
		_lexicon.bgm = LexiconFactory::load(s);
		_config->get_value("lexicon_bigram_sgt", s);
		_lexicon.bgm_sgt = LexiconFactory::load(s);
		_config->get_value("ele_lambda", _ele.lambda);
	}
	~CNLexizer()
	{
		delete _lexicon.ugm;
		delete _lexicon.ugm_sgt;
		delete _lexicon.bgm;
		delete _lexicon.bgm_sgt;
		delete _config;
	}

protected:
	static const size_t _max_token_length = 16;
	static const size_t _max_ngram = 3;
	char _token[_max_ngram][((_max_token_length + 1)<< 2)]; /* x4 for unicode, +4 for space*/
	char _ngm[(_max_token_length << 2) * (_max_ngram + 1)]; 
	IConfig *_config;
	std::string _result;
	struct {
		ILexicon *ugm;
		ILexicon *ugm_sgt;
		ILexicon *bgm;
		ILexicon *bgm_sgt;
	} _lexicon;
	
	struct {
		double lambda;
	} _ele;

	CNLexizer() :_config(NULL) {}

	double _katz_backoff(int vt, int vg, int ugm_num_term, int ugm_num_type, int bgm_num_term, int bgm_num_type)
	{
		int katz_threshold = 0;
		double alpha, delta;
		if (vg > katz_threshold) {
			delta = _ele_estimate(vg, bgm_num_term, bgm_num_type);
			return delta + (log(vg) - log(vt));
		} else {
			return log(alpha) + _ele_estimate(vt, ugm_num_term, ugm_num_type);
		}
	}

	double _ele_estimate(int v, int n, int t)
	{
		return log(v + _ele.lambda) - log(n + t * _ele.lambda);
	}

	size_t _postfix(std::string &s, char *t) 
	{
		const char *p, *start;
		size_t i;
		start = s.c_str();
		for (p = start, i = 0; *p; p++) {
			if (*p == ' ') {
				if (p > start && isalnum(*(p - 1)) && isalnum(*(p + 1))) continue;
				else if (*(p + 1) == '\0') continue;
			}
			t[i++] = *p;	
		}
		t[i] = '\0';

		return i;
	}
};

#endif
