#ifndef KEA_PREPARE_RANKER_HXX
#define KEA_PREPARE_RANKER_HXX

#include "kea_doc.hxx"
#include "token_dict.hxx"
#include "ranker.hxx"

namespace bamboo { namespace kea {

class PrepareRanker : public Ranker{
protected:
	TokenDict * _token_dict;
	double _title_weight;

public:
	PrepareRanker(IConfig * config);
	~PrepareRanker();

	void rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n);
};

}} //end of namespace bamboo::kea

#endif //end of KEA_PREPARE_RANKER_HXX
