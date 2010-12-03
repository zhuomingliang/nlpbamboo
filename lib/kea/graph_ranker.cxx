#include "graph_ranker.hxx"
#include "udgraph.hxx"
#include <algorithm>

namespace bamboo { namespace kea {

GraphRanker::GraphRanker(IConfig * config)
	:_token_aff_dict(NULL)
{
	config->get_value("ke_wordrank_eta", _eta);
	config->get_value("ke_wordrank_alpha", _alpha);
	config->get_value("ke_wordrank_beta", _beta);
	config->get_value("ke_wordrank_maxiter", _maxiter);

	_token_aff_dict = &TokenAffDict::get_instance();
}

GraphRanker::~GraphRanker() {
}

void GraphRanker::rank(YCDoc &doc, std::map<int, double> &token_rank, int top_n) {
	TermIndex index;
	UDGraph * udg_sent;
	_index_term(doc, index);
	_build_sent_graph(doc, index, udg_sent);

	/*
	 * start iteration
	 */
	size_t i, size = doc.sent_list.size();

	if(doc.sent_list.size() == 0)
		return ;

	double * sent_rank = new double[size];
	double * last_sent_rank = new double[size];
	std::map<int, double> last_token_rank;
	std::map<int, double>::iterator it;

	//init weight
	for(i=0; i<size; ++i) {
		sent_rank[i] = 1.0;
		last_sent_rank[i] = 1.0;
	}
	for(it=token_rank.begin(); it!=token_rank.end(); ++it) {
		last_token_rank.insert(last_token_rank.end(), *it);
	}

	for(i=0; i<_maxiter; ++i) {
		_do_iter(doc, index, udg_sent, token_rank, sent_rank,
			last_token_rank, last_sent_rank);
		if(!_should_iter_stop(token_rank, sent_rank,
				last_token_rank, last_sent_rank, size)) {
			std::copy(sent_rank, sent_rank+size,
					last_sent_rank);
			last_token_rank.clear();
			for(it=token_rank.begin();
					it!=token_rank.end(); ++it)
				last_token_rank.insert(last_token_rank.end(), *it);
		} else break;
	}
	/*
	 * end iteration
	 */

	delete [] last_sent_rank;
	delete [] sent_rank;
	delete udg_sent;
}

void GraphRanker::_do_iter(YCDoc &doc, TermIndex &index, UDGraph * udg_sent,
		std::map<int,double> &token_rank, double *sent_rank,
		std::map<int,double> &last_token_rank, double *last_sent_rank) {
	int i, j, num_of_sent = doc.sent_list.size();
	double total_sent_rank = 0, total_token_rank = 0;

	//compute rank of sentences
	for(i=0; i<num_of_sent; ++i) {
		sent_rank[i] = 0;
		for(j=0; j<num_of_sent; ++j) {
			double norm_factor, aff_ss;
			norm_factor = udg_sent->get_norm_factor(j);
			if( norm_factor>0 )
				aff_ss = udg_sent->get_edge(i, j) / norm_factor;
			else
				aff_ss = 0;
			sent_rank[i] += _alpha * last_sent_rank[j] * aff_ss;
		}

		YCSentence * stc = doc.sent_list[i];
		std::map<int,int>::iterator sit;
		for(sit=stc->token_id_list.begin(); sit!=stc->token_id_list.end(); ++sit) {
			int id = sit->first;
			int freq = sit->second;
			double aff_ts = double(freq)/double(index[id].freq);
			sent_rank[i] += _beta * aff_ts * last_token_rank[id];
		}
		total_sent_rank += sent_rank[i];
	}


	if(total_sent_rank != 0)
		for(i=0; i<num_of_sent; ++i) sent_rank[i] /= total_sent_rank;

	//compute rank of tokens
	TermIndex::iterator term_it;
	std::map<int,double>::iterator t;
	for(t=token_rank.begin(), term_it=index.begin();
		t!=token_rank.end(), term_it!=index.end();
		++t, ++term_it) {
		assert(t->first == term_it->first);
		t->second = 0;
		int term_id = t->first;
		int term_sf = term_it->second.sf;
		
		std::map<int, int>::iterator pit;
		for(pit=term_it->second.postinglist.begin();
			pit!=term_it->second.postinglist.end(); ++pit) {
			int sent_id = pit->first;
			int freq = pit->second;

			double aff_st = double(freq) * (1 + log(num_of_sent) - log(term_sf));
			aff_st /= doc.sent_list[sent_id]->rel_weight;
			t->second += _beta * aff_st * last_sent_rank[sent_id];
		}

		std::map<int,double>::iterator tj;
		for(tj=last_token_rank.begin(); tj!=last_token_rank.end(); ++tj) {
			if(term_id == tj->first) continue;

			double aff_ji;
			//aff_ji = _token_aff_dict->get_aff(tj->first, term_id);
			aff_ji = _token_aff_dict->get_aff(doc.token_id_map[tj->first], tj->first,
				doc.token_id_map[term_id], term_id);
			t->second += _alpha * aff_ji * tj->second;
		}

		total_token_rank += t->second;
	}

	if(total_token_rank != 0)
		for(t=token_rank.begin(); t!=token_rank.end(); ++t) t->second /= total_token_rank;
}

bool GraphRanker::_should_iter_stop(std::map<int,double> &token_rank, double *sent_rank,
		std::map<int,double> &last_token_rank, double *last_sent_rank, int num_sent) {
	int i, len=num_sent;
	for(i=0; i<len; ++i) {
		float diff_tmp = fabs(sent_rank[i]-last_sent_rank[i]);
		if(diff_tmp > _eta) return false;
	}

	std::map<int,double>::iterator cur, last;
	for(cur=token_rank.begin(), last=last_token_rank.begin();
		cur!=token_rank.end(), last!=last_token_rank.end();
		++cur,++last) {
		float diff_tmp = fabs(cur->second-last->second);
		if(diff_tmp > _eta) return false;
	}

	return true;
}

void GraphRanker::_build_sent_graph(YCDoc &doc, TermIndex &index, UDGraph *& udg_sent) {
	size_t i, j, num_of_sent = doc.sent_list.size();

	if(num_of_sent == 0) return;

	udg_sent = new UDGraph(num_of_sent+1);

	for(i=0; i<num_of_sent-1; ++i)
		for(j=i+1; j<num_of_sent; ++j) {
			double aff = _sent_aff(doc.sent_list[i], doc.sent_list[j], num_of_sent, index);

			udg_sent -> add_edge(i, j, aff);
		}
	for(i=0; i<num_of_sent; ++i) {
		YCSentence * sent =  doc.sent_list[i];
		std::map<int,int>::iterator it
			= sent->token_id_list.begin();
		for(; it != sent->token_id_list.end(); ++it) {
			int id = it->first;
			int freq = it->second;
			int sf = index[id].sf;
			double ISF = 1 + log(num_of_sent) - log(sf);
			sent->rel_weight += ISF * double(freq);
		}
	}
}

double GraphRanker::_sent_aff(YCSentence *lst, YCSentence *rst, int num_of_sent, TermIndex &index) {
	double score = 0;
	std::map<int,int>::iterator l, r, l_end, r_end;
	l = lst->token_id_list.begin();
	r = rst->token_id_list.begin();
	l_end = lst->token_id_list.end();
	r_end = rst->token_id_list.end();

	while(l!=l_end && r!=r_end) {
		if(l->first < r->first) {
			++l;
		} else if(l->first > r->first) {
			++r;
		} else {
			int sf = index[l->first].sf;
			assert(sf>0);
			double ISF = 1 + log(double(num_of_sent)/double(sf));
			score += (l->second*r->second)*ISF*ISF;
			++l;
			++r;
		}
	}
	
	return score;	
}

void GraphRanker::_index_term(YCDoc &doc, TermIndex &index) {
	size_t i, size;
	size = doc.sent_list.size();
	for(i=0; i<size; ++i) {
		YCSentence * sent = doc.sent_list[i];
		std::map<int,int>::iterator tm_it
			= sent->token_id_list.begin();
		for(; tm_it != sent->token_id_list.end(); ++tm_it) {
			int tid = tm_it->first;
			int freq = tm_it->second;

			TermIndex::iterator ti_it
				= index.lower_bound(tid);
			if(ti_it != index.end() && ti_it->first == tid) {
				ti_it->second.sf += 1;
				ti_it->second.freq += freq;
				ti_it->second.postinglist.insert(std::make_pair(i, freq));
			} else {
				TermInfo tmp;
				tmp.sf = 1;
				tmp.freq = freq;
				tmp.postinglist.insert(std::make_pair(i, freq));
				index.insert(std::make_pair(tid, tmp));
			}
		}
	}
}

}} //end of namespace bamboo::kea
