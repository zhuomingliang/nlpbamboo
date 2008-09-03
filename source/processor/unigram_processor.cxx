#include "lexicon_factory.hxx"
#include "unigram_processor.hxx"
#include "utf8.hxx"
#include <iostream>

UnigramProcessor::UnigramProcessor(IConfig *config)
	:_lambda(0.5), _max_token_length(8)
{
	const char *s;

	config->get_value("ele_lambda", _lambda);
	config->get_value("lexicon_unigram", s);
	config->get_value("max_token_length", _max_token_length);
	_lexicon = LexiconFactory::load(s);

	_token = new char[_max_token_length << 2 + 1]; /* x4 for unicode */
}

UnigramProcessor::~UnigramProcessor()
{
	delete _token;
	delete _lexicon;
}

int UnigramProcessor::process(std::vector< std::pair<std::string, LexAttribute> > &prod)
{
	std::vector< std::pair<std::string, LexAttribute> >::iterator it;

	_out.clear();
	for (it = prod.begin(); it < prod.end(); it++) {
		_lexize(*it);
		prod.erase(it);	
	}

	for (it = _out.end() - 1; it >= _out.begin(); it--) 
		prod.push_back(*it);
}


size_t UnigramProcessor::_lexize(std::pair<std::string, LexAttribute> &term)
{
	size_t i, j, k, length, max_token_length, *backref;
	double *score, lp;
	size_t num_terms, num_types;
	const char *s;

	s = term.first.c_str();
	num_terms = _lexicon->sum_value();
	num_types = _lexicon->num_insert();

	length = utf8::length(s);
	score = new double[length + 1];
	backref = new size_t[length + 1];

	for (i = 0; i <= length; i++) {
		score[i] = -1e300;
		backref[i] = 0;
	}

	/* Calculate score using DP */
	score[0] = 0;
	for (i = 0; i < length; i++) {
		max_token_length = (_max_token_length < length - i)?_max_token_length:length - i;
		bool found = false;
		for (j = 1; j <= max_token_length; j++) {
			k = utf8::sub(_token, s, i, j);
			int v = _lexicon->search(_token);
			if (v > 0) {
				lp = _ele_estimate(v, num_terms, num_types);
				if (score[i + j] < score[i] + lp) {
					score[i + j] = score[i] + lp;
					backref[i + j] = i;
					found = true;
				}
			}
		}

		if (!found) {
			lp = _ele_estimate(0, num_terms, num_types);
			j = 1;
			if (score[i + j] < score[i] + lp) {
				score[i + j] = score[i] + lp;
				backref[i + j] = i;
			}
		}
	}
	for (k = 0, i = length; i > 0;) {
		k = utf8::sub(_token, s, backref[i], i - backref[i]);
		_out.push_back(std::pair<std::string, LexAttribute>(_token, NULL));
		i = backref[i];
	}
	delete []score;
	delete []backref;

	return 0;
}
