#ifndef KEA_TOKEN_AFF_DICT_HXX
#define KEA_TOKEN_AFF_DICT_HXX

#include "kea_mmap.hxx"
#include "kea_hash.hxx"
#include "token_dict.hxx"

namespace bamboo { namespace kea {

class TokenAffDict {
protected:
	TokenDict * _token_dict;
	YCHash<double> *_aff_ht;
	bool _is_init;

public:
	TokenAffDict():_token_dict(NULL),_aff_ht(NULL),_is_init(false) {}
	~TokenAffDict() {
		if(_aff_ht) {
			delete _aff_ht;
			_aff_ht = NULL;
		}
	}

	static TokenAffDict & get_instance() {
		static TokenAffDict dict;
		return dict;
	}

	int init(const char * token_aff_dict) {

		_token_dict = &TokenDict::get_instance();
		_aff_ht = new YCHash<double>(token_aff_dict);
		_is_init = true;

		return 0;
	}

	double get_aff(const char * k1, int id1, const char * k2, int id2) {
		if(!_is_init) {
			return 0;
		}

		int max_id = _token_dict->get_max_id() + 1;
		long long uniq_id = (long long)id1 * (long long)max_id + (long long)id2;
		YCHashNode<double> * p = _aff_ht->search(k1, k2, uniq_id);
		if(!p) {
			return 0;
		}
		return p->get_value();
	}
};

}} //end of namespace bamboo::kea

#endif //end of KEA_TOKEN_AFF_DICT_HXX
