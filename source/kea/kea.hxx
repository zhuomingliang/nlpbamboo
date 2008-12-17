#ifndef KEA_KEA_HXX
#define KEA_KEA_HXX

#include "config_factory.hxx"
#include "text_parser.hxx"
#include "kea_doc.hxx"
#include "segment_tool.hxx"
#include "prepare_ranker.hxx"
#include "graph_ranker.hxx"
#include "tfidf_ranker.hxx"
#include <vector>
#include <string>

namespace bamboo { namespace kea {

class KeywordExtractor {
protected:
	enum Algorithm {
		tfidf,
		graph
	};

	IConfig *_config;
	TextParser * _text_parser;
	PrepareRanker * _prepare_ranker;
	GraphRanker * _graph_ranker;
	TfidfRanker * _tfidf_ranker;
	int _top_n;
	int _verbose;
	double _title_score;
	double _ner_score;
	Algorithm _algo;

public:

	KeywordExtractor(IConfig * config);
	~KeywordExtractor();

	int get_keyword(const char * text, std::vector<std::string> &);
	int get_keyword(const char * title, const char * text, std::vector<std::string> &);

	int max_keywords();

};

}} //end of namespace bamboo::kea

#endif //end of KEA_KEA_HXX
