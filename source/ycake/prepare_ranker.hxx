#ifndef YCAKE_PREPARE_RANKER_HXX
#define YCAKE_PREPARE_RANKER_HXX

#include "ycake_doc.hxx"
#include "token_dict.hxx"
#include "ranker.hxx"

namespace bamboo { namespace ycake {

class PrepareRanker : public Ranker{
protected:
	TokenDict * _token_dict;
	double _title_weight;

public:
	PrepareRanker(IConfig * config);
	~PrepareRanker();

	void rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n);
};

}} //end of namespace bamboo::ycake

#endif //end of YCAKE_PREPARE_RANKER_HXX
