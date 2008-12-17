#include "kea.hxx"
#include "token_filter.hxx"
#include <algorithm>
#include <cstdlib>

namespace bamboo { namespace kea {

struct pair_cmp {
	bool operator() (std::pair<int, double> &ls, std::pair<int, double> &rs) {
		return ls.second > rs.second;
	}
};

KeywordExtractor::KeywordExtractor(IConfig * config)
	:_config(config),_verbose(0)
{
	const char * s;
	double title_weight, ner_weight, firstocc_w, numocc_w;

	_config->get_value("verbose", _verbose);

	SegmentTool & seg_tool = SegmentTool::get_instance();
	seg_tool.init(_config);

	_config->get_value("ke_token_aff_dict", s);
	if(*s == 0) {
		throw std::runtime_error("ke_token_aff_dict is null");
	}
	TokenAffDict & token_aff_dict = TokenAffDict::get_instance();
	token_aff_dict.init(s);

	TokenDict & token_dict = TokenDict::get_instance();
	token_dict.init(_config);

	TokenFilter & token_filter = TokenFilter::get_instance();
	token_filter.init(_config);

	_text_parser = new TextParser(_config);
	_prepare_ranker = new PrepareRanker(_config);
	_graph_ranker = new GraphRanker(_config);
	_tfidf_ranker = new TfidfRanker(_config);

	_config->get_value("ke_top_n", _top_n);

	_config->get_value("ke_title_weight", title_weight);
	_config->get_value("ke_ner_weight", ner_weight);
	_config->get_value("ke_firstocc_w", firstocc_w);
	_config->get_value("ke_numocc_w", numocc_w);

	_title_score = 2 * title_weight / ( firstocc_w + numocc_w );
	_ner_score = 2 * ner_weight / ( firstocc_w + numocc_w );

	_algo = graph;
	_config->get_value("ke_algorithm", s);
	if(!strcmp(s, "tfidf")) {
		_algo = tfidf;
	} else if(!strcmp(s, "graph")) {
		_algo = graph;
	} else {
		throw std::runtime_error("unexpected algorithm of keyword extraction.");
	}
}

KeywordExtractor::~KeywordExtractor() {
	delete _text_parser;
	delete _prepare_ranker;
	delete _graph_ranker;
	delete _tfidf_ranker;
	delete _config;
}

int KeywordExtractor::get_keyword(const char * text, std::vector<std::string> & words) {
	return get_keyword(NULL, text, words);
}

int KeywordExtractor::get_keyword(const char * title, const char * text, std::vector<std::string> & words) {
	YCDoc doc;
	_text_parser->parse_text(title, text, doc);

	std::map<int, double> token_map;

	if(_algo==graph) {
		_prepare_ranker->rank(doc, token_map, _top_n);
		_graph_ranker->rank(doc, token_map, _top_n);
	} else {
		_prepare_ranker->rank(doc, token_map, _top_n);
		_tfidf_ranker->rank(doc, token_map, _top_n);
	}

	TokenDict & td = TokenDict::get_instance();
	std::vector<std::pair<int, double> > res;
	std::map<int, double>::iterator tm_it
		= token_map.begin();
	for(; tm_it != token_map.end(); ++tm_it) {
		double score = tm_it -> second;
		score *= td.get_idf(doc.token_id_map[tm_it->first]);
		res.push_back(std::make_pair(tm_it->first, score));
	}
	//copy(token_map.begin(), token_map.end(), back_inserter(res));

	int res_size = res.size();
	int i, N = (_top_n*2) < res_size ? (_top_n*2) : res_size;
	std::partial_sort(res.begin(), res.begin() + N, res.end(), pair_cmp());
	for(i=0; i<N; ++i) {
		int id = res[i].first;
		double score = res[i].second;
		if(doc.token_in_title.count(id) > 0)
			score *= _title_score;
		else if(doc.token_ner.count(id) > 0)
			score *= _ner_score;
		res[i].second = score;
	}

	N = _top_n < res_size ? _top_n : res_size;
	std::partial_sort(res.begin(), res.begin() + N, res.end(), pair_cmp());
	for(i=0; i<N; ++i) {
		words.push_back(doc.token_id_map[res[i].first]);
	}

	return 0;
}

int KeywordExtractor::max_keywords() {
	return _top_n;
}

}} //end of namespace bamboo::kea
