#ifndef KEA_GRAPH_RANKER_HXX
#define KEA_GRAPH_RANKER_HXX

#include "kea_doc.hxx"
#include "ranker.hxx"
#include "udgraph.hxx"
#include "token_aff_dict.hxx"

namespace bamboo { namespace kea {

class GraphRanker : public Ranker{
protected:
	typedef struct {
		int sf; //term's Sentence Frequency
		int freq; //term freq in all sentences
		std::map<int, int> postinglist; //sentence s, term freq in s
	} TermInfo;
	typedef std::map<int, TermInfo> TermIndex;

protected:
	double _alpha;
	double _beta;
	double _eta;
	double _maxiter;

	TokenAffDict * _token_aff_dict;

protected:
	void _index_term(YCDoc &doc, TermIndex &index);
	double _sent_aff(YCSentence *lst, YCSentence *rst, int, TermIndex &);
	void _build_sent_graph(YCDoc &doc, TermIndex &index, UDGraph *& udg_sent);
	void _do_iter(YCDoc &doc, TermIndex &index, UDGraph * udg_sent,
		std::map<int,double> &token_rank, double *sent_rank,
		std::map<int,double> &last_token_rank, double *last_sent_rank);
	bool _should_iter_stop(std::map<int,double> &token_rank, double *sent_rank,
		std::map<int,double> &last_token_rank, double *last_sent_rank, int num_sent);

public:
	GraphRanker(IConfig * config);
	~GraphRanker();

	void rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n);
};

}} //end of namespace bamboo::kea

#endif //end of KEA_GRAPH_RANKER_HXX
