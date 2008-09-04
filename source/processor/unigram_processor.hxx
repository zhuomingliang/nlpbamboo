#ifndef UNIGRAM_HXX
#define UNIGRAM_HXX

#include <math.h>
#include <stack>
#include "lex_token.hxx"
#include "iprocessor.hxx"
#include "ilexicon.hxx"

class UnigramProcessor: public Processor {
protected:
	ILexicon *_lexicon;
	double _lambda;
	int _max_token_length;
	char *_token;
	std::stack<LexToken *> stack;

	UnigramProcessor();
	size_t _unigram_model(LexToken *token);
	double _ele_estimate(int v, int n, int t)
	{
			return log(v + _lambda) - log(n + t * _lambda);
	}
	bool _can_process(LexToken *token) 
	{
		return (token->get_attr() == LexToken::attr_unknow);
	}
	void _process(LexToken *token, std::vector<LexToken *> &out);

public:
	UnigramProcessor(IConfig *config);
	~UnigramProcessor();
};

#endif // UNIGRAM_HXX
