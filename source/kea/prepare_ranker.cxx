#include "prepare_ranker.hxx"
#include <set>
#include <cmath>

namespace bamboo { namespace kea {


PrepareRanker::PrepareRanker(IConfig * config)
	:_token_dict(NULL)
{
	_token_dict = &TokenDict::get_instance();
	config->get_value("ke_title_weight", _title_weight);
	config->get_value("ke_ner_weight", _ner_weight);
	config->get_value("ke_firstocc_w", _firstocc_w);
	config->get_value("ke_firstocc_t", _firstocc_t);
	config->get_value("ke_numocc_w", _numocc_w);
	config->get_value("ke_numocc_s", _numocc_s);
	config->get_value("ke_numocc_t", _numocc_t);
}
	
PrepareRanker::~PrepareRanker() {
}

void PrepareRanker::rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n) {
	YCToken * tok;
	int oov_token_id_base = _token_dict->get_max_id() + 1;
	std::map<int, char *> & token_id_map
		= doc.token_id_map;
	YCDoc::oov_map & oov_token = doc.oov_token;
	std::map<int, int> token_num_occ, token_first_occ;
	std::map<int, int>::iterator iter;
	int total_occ = 0;

	std::vector<YCSentence *>::iterator ycs_it
		= doc.sent_list.begin();
	for(; ycs_it != doc.sent_list.end(); ++ycs_it) {
		std::vector<YCToken *>::iterator yct_it
			= (*ycs_it)->token_list.begin();
		for(; yct_it != (*ycs_it)->token_list.end(); ++yct_it) {
			tok = *yct_it;
			int tok_id = _token_dict->get_id(tok->get_token());
			if(tok_id == 0) {
				YCDoc::oov_map::iterator om_it
					= oov_token.lower_bound(tok->get_token());
				if(om_it != oov_token.end()
					&& !strcmp(om_it->first, tok->get_token())) {
					tok_id = om_it->second;
				} else {
					oov_token.insert(om_it,
						std::make_pair(tok->get_token(),
						oov_token_id_base));
					tok_id = oov_token_id_base++;
				}
			}
			tok->set_id(tok_id);
			(*ycs_it)->token_id_list[tok_id] += 1;
			std::map<int, char *>::iterator tim_it = token_id_map.find(tok_id);
			if(tim_it == token_id_map.end()
				|| tim_it->first != tok_id) {
				token_id_map.insert(tim_it,
					std::make_pair(tok_id, tok->get_token()));
			}

			token_num_occ[tok_id] += 1;

			if(tok->get_pos() != 0) {
				doc.token_ner.insert(tok_id);
			}

			if((*ycs_it)->is_title) {
				doc.token_in_title.insert(tok_id);
			} else {
				iter = token_first_occ.lower_bound(tok_id);
				if(iter == token_first_occ.end()
						|| iter->first != tok_id) {
					token_first_occ.insert(iter,
							std::make_pair(tok_id, total_occ + 1));
				}
				total_occ++;
			}

			/*double weight = _token_dict->get_idf(tok->get_token());
			if((*ycs_it)->is_title) weight *= _title_weight;
			token_rank[tok_id] += weight;

			(*ycs_it)->abs_weight += weight;*/
		}
	}

	int elem_in_block = total_occ / MAX_BLOCK;
	if(total_occ % MAX_BLOCK != 0 || elem_in_block == 0) ++elem_in_block;

	int tok_id, num_occ, first_occ;
	double weight;
	for(iter = token_num_occ.begin();
			iter != token_num_occ.end(); ++iter) {

		tok_id = iter->first;
		num_occ = iter->second;
		first_occ = token_first_occ[tok_id];
		first_occ /= elem_in_block;
		if(first_occ % elem_in_block != 0) first_occ++;

		weight = 0;

		if(first_occ > 0)
			weight += _firstocc_w * exp(- (double)first_occ / _firstocc_t);

		weight += _numocc_w * log1p((double)num_occ / _numocc_s ) + _numocc_t;

		if(doc.token_in_title.count(tok_id)>0)
			weight += _title_weight;

		if(doc.token_ner.count(tok_id) > 0)
			weight += _ner_weight;

		weight *= _token_dict->get_idf(doc.token_id_map[tok_id]);

		token_rank[tok_id] = weight;
	}
}

}} // end of namespace bamboo::kea
