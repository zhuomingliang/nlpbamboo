#ifndef KEA_PREPARE_RANKER_HXX
#define KEA_PREPARE_RANKER_HXX

#include "kea_doc.hxx"
#include "token_dict.hxx"
#include "ranker.hxx"

namespace bamboo { namespace kea {

#define MAX_BLOCK 256

class PrepareRanker : public Ranker{
protected:
	TokenDict * _token_dict;
	double _title_weight;
	double _ner_weight;
	double _firstocc_w;
	double _firstocc_t;
	double _numocc_w;
	double _numocc_s;
	double _numocc_t;

public:
	PrepareRanker(IConfig * config);
	~PrepareRanker();

	void rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n);
};

}} //end of namespace bamboo::kea

#endif //end of KEA_PREPARE_RANKER_HXX
