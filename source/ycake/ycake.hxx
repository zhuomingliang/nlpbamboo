#ifndef YCAKE_YCAKE_HXX
#define YCAKE_YCAKE_HXX

#include "config_factory.hxx"
#include "text_parser.hxx"
#include "ycake_doc.hxx"
#include "segment_tool.hxx"
#include "prepare_ranker.hxx"
#include "graph_ranker.hxx"
#include "tfidf_ranker.hxx"
#include <vector>
#include <string>

namespace bamboo { namespace ycake {

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
	Algorithm _algo;

public:

	KeywordExtractor(const char * config);
	~KeywordExtractor();

	int get_keyword(const char * text, std::vector<std::string> &);
	int get_keyword(const char * title, const char * text, std::vector<std::string> &);

protected:
	void _lazy_create_config(const char *custom);

};

}} //end of namespace bamboo::ycake

#endif //end of YCAKE_YCAKE_HXX
