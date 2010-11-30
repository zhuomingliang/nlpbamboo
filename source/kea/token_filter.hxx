#ifndef KEA_TOKEN_FILTER_HXX
#define KEA_TOKEN_FILTER_HXX

#include "lexicon_factory.hxx"
#include "ilexicon.hxx"
#include "datrie.hxx"
#include "config_factory.hxx"
#include "kea_doc.hxx"
#include "utf8.hxx"
#include <cctype>

namespace bamboo { namespace kea {

class TokenFilter {
protected:
	bamboo::ILexicon * _filter_dict;
	bool _is_init;

protected:
	int _feature_min_length;
	int _feature_min_utf8_length;

public:
	TokenFilter():_filter_dict(NULL),_is_init(false) {}
	~TokenFilter() {
		if(_filter_dict) delete _filter_dict;
	}

	int init(IConfig * config) {
		const char * s;
		config->get_value("ke_filter_dict", s);
		if(*s == 0) {
			throw std::runtime_error("ke_filter_dict is null");
		}
		_filter_dict = LexiconFactory::load(s);

		config->get_value("ke_feature_min_length", _feature_min_length);
		config->get_value("ke_feature_min_utf8_length", _feature_min_utf8_length);

		_is_init = true;
		return 0;
	}

	bool is_init() {
		return _is_init;
	}

	static TokenFilter & get_instance() {
		static TokenFilter dict;
		return dict;
	}

	bool is_filter_word(YCToken * token) {
		const char * word = token->get_token();

		if(token->get_pos() == 0 && _rule_filter(word)) {
			return true;
		}

		if(!_is_init || !_filter_dict) {
			return false;
		}

		int val = _filter_dict->search(word);

		if(val > 0)
			return true;

		return false;
	}

protected:
	bool _rule_filter(const char *token) {
		if((int)strlen(token) < _feature_min_length)
			return true;

		size_t step, len = 0;
		char uch[8], cch;
		const char * p = token;
		for(; *p; p+=step) {
			step = utf8::first(p, uch);
			cch = utf8::dbc2sbc(uch, step);
			if(ispunct(cch) || isdigit(cch)) {
				return true;
			}
			len++;
		}
		if((int)len < _feature_min_utf8_length) {
			return true;
		}

		return false;
	}

};

}} // end of namespace bamboo::kea

#endif // end of KEA_TOKEN_FILTER_HXX
