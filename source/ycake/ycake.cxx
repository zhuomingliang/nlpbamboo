#include "ycake.hxx"

namespace bamboo { namespace ycake {

struct pair_cmp {
	bool operator() (std::pair<int, double> &ls, std::pair<int, double> &rs) {
		return ls.second > rs.second;
	}
};

KeywordExtractor::KeywordExtractor(const char * config)
	:_config(NULL),_verbose(0)
{
	_lazy_create_config(config);

	const char * s, * t;
	_config->get_value("verbose", _verbose);

	SegmentTool & seg_tool = SegmentTool::get_instance();
	seg_tool.init(_config);

	_config->get_value("token_aff_dict", s);
	TokenAffDict & token_aff_dict = TokenAffDict::get_instance();
	token_aff_dict.init(s);

	_config->get_value("token_id_dict", s);
	_config->get_value("token_df_dict", t);
	TokenDict & token_dict = TokenDict::get_instance();
	token_dict.init(s, t);

	_text_parser = new TextParser(_config);
	_prepare_ranker = new PrepareRanker(_config);
	_graph_ranker = new GraphRanker(_config);
	_tfidf_ranker = new TfidfRanker(_config);

	_config->get_value("top_n", _top_n);

	_algo = graph;
	_config->get_value("algorithm", s);
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

void KeywordExtractor::_lazy_create_config(const char * custom) {
	std::vector<std::string>::size_type i;
	bool flag = false;
	struct stat buf;
	std::vector<std::string> v;

	if (custom)
		v.push_back(custom);
	v.push_back("ycake.cfg");
	v.push_back("etc/ycake.cfg");
	v.push_back("/opt/bamboo/etc/ycake.cfg");
	v.push_back("/etc/ycake.cfg");
	for (i = 0; i < v.size(); i++) {
		if (_verbose)
			std::cerr << "loading configuration " << v[i] << " ... ";
		if (stat(v[i].c_str(), &buf) == 0) {
			if (_verbose) std::cerr << "found." << std::endl;
			_config = ConfigFactory::create(v[i].c_str());
			flag = true;
			break;
		} else {
			if (_verbose) std::cerr << "not found." << std::endl;
		}
	}

	if (!flag)
		throw std::runtime_error("can not find ycake configuration");
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

	std::vector<std::pair<int, double> > res;
	copy(token_map.begin(), token_map.end(), back_inserter(res));

	int res_size = res.size();
	int i, N = _top_n < res_size ? _top_n : res_size;
	std::partial_sort(res.begin(), res.begin() + N, res.end(), pair_cmp());

	for(i=0; i<N; ++i) {
		words.push_back(doc.token_id_map[res[i].first]);
	}

	return 0;
}

}} //end of namespace bamboo::ycake
