#ifndef YCAKE_TFIDF_RANKER_HXX
#define YCAKE_TFIDF_RANKER_HXX

#include "ycake_doc.hxx"
#include "ranker.hxx"

namespace bamboo { namespace ycake {

class TfidfRanker : public Ranker{
public:
	TfidfRanker(IConfig * config) {}
	~TfidfRanker() {}

	void rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n);
};

}} //end of namespace bamboo::ycake

#endif //end of YCAKE_TFIDF_RANKER_HXX
