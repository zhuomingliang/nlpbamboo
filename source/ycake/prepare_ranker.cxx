#include "prepare_ranker.hxx"

namespace bamboo { namespace ycake {


PrepareRanker::PrepareRanker(IConfig * config)
	:_token_dict(NULL)
{
	_token_dict = &TokenDict::get_instance();
	config->get_value("ke_title_weight", _title_weight);
}
	
PrepareRanker::~PrepareRanker() {
}

void PrepareRanker::rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n) {
	YCToken * tok;
	int oov_token_id_base = _token_dict->get_max_id() + 1;
	std::map<int, char *> & token_id_map
		= doc.token_id_map;
	YCDoc::oov_map & oov_token = doc.oov_token;

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

			double weight = _token_dict->get_idf(tok->get_token());
			if((*ycs_it)->is_title) weight *= _title_weight;
			token_rank[tok_id] += weight;

			(*ycs_it)->abs_weight += weight;
		}
	}
}

}} // end of namespace bamboo::ycake
