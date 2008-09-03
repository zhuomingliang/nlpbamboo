#ifndef UNIGRAM_HXX
#define UNIGRAM_HXX

#include "iprocessor.hxx"
#include "ilexicon.hxx"
#include <math.h>

class UnigramProcessor: public IProcessor {
protected:
	std::vector< std::pair<std::string, LexAttribute> > _out;
	ILexicon *_lexicon;
	double _lambda;
	int _max_token_length;
	char *_token;

	UnigramProcessor();
	size_t _lexize(std::pair<std::string, LexAttribute> &term);
	double _ele_estimate(int v, int n, int t)
	{
			return log(v + _lambda) - log(n + t * _lambda);
	}

public:
	UnigramProcessor(IConfig *config);
	~UnigramProcessor();
	int process(std::vector< std::pair<std::string, LexAttribute> > &in);
};

#endif // UNIGRAM_HXX
