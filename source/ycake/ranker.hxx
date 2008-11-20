#ifndef YCAKE_RANKER_HXX
#define YCAKE_RANKER_HXX

#include "ycake_doc.hxx"
#include "token_dict.hxx"
#include "config_factory.hxx"

namespace bamboo { namespace ycake {

class Ranker {
public:

	Ranker() {}
	Ranker(IConfig * config) {}
	virtual ~Ranker() {}

public:
	virtual void rank(YCDoc & doc, std::map<int, double> & token_rank, int top_n) {}

};

}} //end of namespace bamboo::ycake

#endif //end of YCAKE_RANKER_HXX
