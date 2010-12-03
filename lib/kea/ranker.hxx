#ifndef KEA_RANKER_HXX
#define KEA_RANKER_HXX

#include "kea_doc.hxx"
#include "token_dict.hxx"
#include "config_factory.hxx"

namespace bamboo { namespace kea {

class Ranker {
public:

	Ranker() {}
	Ranker(IConfig * config) {}
	virtual ~Ranker() {}

public:
	virtual void rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n) {}

};

}} //end of namespace bamboo::kea

#endif //end of KEA_RANKER_HXX
