#ifndef KEA_TFIDF_RANKER_HXX
#define KEA_TFIDF_RANKER_HXX

#include "kea_doc.hxx"
#include "ranker.hxx"

namespace bamboo { namespace kea {

class TfidfRanker : public Ranker{
public:
	TfidfRanker(IConfig * config) {}
	~TfidfRanker() {}

	void rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n);
};

}} //end of namespace bamboo::kea

#endif //end of KEA_TFIDF_RANKER_HXX
