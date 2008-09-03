#include <ctype.h>
#include <cstring>
#include <cmath>

#include "cnlexizer.hxx"
#include "utf8.hxx"

double CNLexizer::ugm_score(const char *s)
{
	size_t i = 0, length, anchor;
	double score;

	length = utf8::length(s);
	for (i = 0, anchor = 0, score = 0; i < length; i++) {
		utf8::sub(_token[0], s, i, 1);
		if (strcmp(_token[0], " ") == 0 && i > 0) {
			utf8::sub(_token[0], s, anchor, i - anchor);
			score += _lexicon.ugm->search(_token[0]);
			anchor = i + 1;
		}
	}

	return score;
}

size_t CNLexizer::maxforward(char *t, const char *s)
{
	size_t length, max_token_length, i, j, k;
	
	if (s == NULL) throw std::runtime_error("invalid input");
	length = utf8::length(s);
	_result.erase();
	for (i = 0; i < length; i++) {
		max_token_length = (_max_token_length < length - i)?_max_token_length:length - i;
		for (k = 0, j = max_token_length; j > 0; j--) {
			k = utf8::sub(_token[0], s, i, j);
			if (_lexicon.ugm->search(_token[0]) > 0) break;
		}
		if (j == 0) {j = 1;}
		_token[0][k] = ' ';
		_token[0][k + 1] = '\0';
		_result.append(_token[0]);
		i = i + j - 1;
	}

	return _postfix(_result, t);
}

size_t CNLexizer::maxbackward(char *t, const char *s)
{
	size_t length, max_token_length, j, k, start;
	int i;
	
	if (s == NULL) throw std::runtime_error("invalid input");
	length = utf8::length(s);
	max_token_length = (_max_token_length < length)?_max_token_length:length;
	_result.erase();

	for (i = length - 1; i > -1; i--) {
		for (start = 0, k = 0, j = max_token_length - 1; j > 0; j--) {
			start = i - j + 1;
			if (start < 0) continue;
			k = utf8::sub(_token[0], s, start, j);
			if (_lexicon.ugm->search(_token[0])) break;
		}
		i = start;
		_token[0][k] = ' ';
		_token[0][k + 1] = '\0';
		_result.insert(0, _token[0]);
	}

	return _postfix(_result, t);
}

size_t CNLexizer::unigram(char *t, const char *s)
{
	size_t i, j, k, length, max_token_length, *backref;
	double *score, lp;
	size_t num_terms, num_types;
	if (s == NULL) throw std::runtime_error("invalid input");
	num_terms = _lexicon.ugm->sum_value();
	num_types = _lexicon.ugm->num_insert();

	length = utf8::length(s);
	score = new double[length + 1];
	backref = new size_t[length + 1];

	for (i = 0; i <= length; i++) {
		score[i] = -1e300;
		backref[i] = 0;
	}

	/* Calculate score using DP */
	score[0] = 0;
	_result.erase();
	for (i = 0; i < length; i++) {
		max_token_length = (_max_token_length < length - i)?_max_token_length:length - i;
		bool found = false;
		for (j = 1; j <= max_token_length; j++) {
			k = utf8::sub(_token[0], s, i, j);
			int v = _lexicon.ugm->search(_token[0]);
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
		k = utf8::sub(_token[0], s, backref[i], i - backref[i]);
		_token[0][k] = ' ';
		_token[0][k + 1] = '\0';
		_result.insert(0, _token[0]);
		i = backref[i];
	}
	delete []score;
	delete []backref;

	return _postfix(_result, t);
}

size_t CNLexizer::bigram(char *t, const char *s)
{
	size_t i, j, k, bytes[2], length, limit[2];
	double *score, lp;
	size_t *backref, *spaceref, nw[2], nt[2], missing;
	char sn[2][128];

	nw[0] = _lexicon.ugm->sum_value();
	nt[0] = _lexicon.ugm->num_insert();
	nw[1] = _lexicon.bgm->sum_value();
	nt[1] = _lexicon.bgm->num_insert();
	missing = nt[0] * nt[0] - nt[1];

	if (s == NULL) throw std::runtime_error("invalid input");
	length = utf8::length(s);

	score = new double[length + 1];
	backref = new size_t[length + 1];
	spaceref = new size_t[length + 1];
	for (i = 0; i <= length; i++) {
		score[i] = -1e300;
		backref[i] = 0;
		spaceref[i] = 0;
	}
	/* Calculate score using DP */
	score[0] = 0;
	_result.erase();
	for (i = 0; i < length; i++) {
		bool found = false;
		int max_vt = 0;
		limit[0] = (_max_token_length < length - i)?_max_token_length:length - i;
		for (j = 1; j <= limit[0]; j++) {
			bytes[0] = utf8::sub(_token[0], s, i, j);
			limit[1] = (_max_token_length < length - i - j)?_max_token_length:length - i - j;
			int vt = _lexicon.ugm->search(_token[0]);
			if (vt > 0) {
				for (k = 1; k <= limit[1]; k++) {
					bytes[1] = utf8::sub(_token[1], s, i + j, k);
					snprintf(_ngm, sizeof(_ngm), "%s %s", _token[0], _token[1]); 
					int vg = _lexicon.bgm->search(_ngm);
					if (vg > 0) {
						snprintf(sn[0], 128, "%d", vg);
						snprintf(sn[1], 128, "%d", vg + 1);
						int d;
						for (d = 0;_lexicon.bgm_sgt->search(sn[0]) < 1 && d < 20;d++) 
							snprintf(sn[0], 128, "%d", vg - d);
						for (d = 1;_lexicon.bgm_sgt->search(sn[1]) < 1 && d < 20;d++) 
							snprintf(sn[1], 128, "%d", vg + d);
						if (_lexicon.bgm_sgt->search(sn[0]) < 1 || _lexicon.bgm_sgt->search(sn[1]) < 1) {
							lp = _ele_estimate(vg, nw[1], nt[1]) - _ele_estimate(vt, nw[0], nt[0]);
						} else {
							lp = log(vg + 1) + log(_lexicon.bgm_sgt->search(sn[1])) - log(_lexicon.bgm_sgt->search(sn[0])) - log(nw[1]) - _ele_estimate(vt, nw[0], nt[0]);
						}
						if (score[i + j + k] < score[i] + lp) {
							score[i + j + k] = score[i] + lp;
							backref[i + j + k] = i;
							spaceref[i + j + k] = j;
							found = true;
						}
					}
				}
				if (max_vt < vt) max_vt = vt;
			}
		}
		if (!found) {
			lp = log(_lexicon.bgm_sgt->search("1")) - log(_lexicon.bgm_sgt->sum_value()) - log(missing) - _ele_estimate(max_vt, nw[0], nt[0]);
			j = 1; k = 1;
			if (score[i + j + k] < score[i] + lp) {
				score[i + j + k] = score[i] + lp;
				backref[i + j + k] = i;
				spaceref[i + j + k] = 1;
			}
		}
		// std::cout << "_ngm = " << _ngm << " = " <<  lp <<std::endl;
	}
	for (k = 0, i = length; i > 0;) {
		k = utf8::sub(_ngm, s, backref[i], i - backref[i]);
		// std::cout << "__ngm = " << _ngm << " - " << spaceref[i] << std::endl;
		k = utf8::sub(_token[0], _ngm, 0, spaceref[i]);
		_token[0][k] = ' ';
		_token[0][k + 1] = '\0';
		// std::cout << "_token[0] = " << _token[0] << std::endl;
		k = utf8::sub(_token[1], _ngm, spaceref[i], (size_t)-1);
		_token[1][k] = ' ';
		_token[1][k + 1] = '\0';
		// std::cout << "_token[1] = " << _token[1] << std::endl;
		_result.insert(0, _token[1]);
		_result.insert(0, _token[0]);
		i = backref[i];
	}
	delete score;
	delete backref;
	delete spaceref;
	return _postfix(_result, t);
}

