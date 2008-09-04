#ifndef MAXFORWARD_PROCESSOR_HXX
#define MAXFORWARD_PROCESSOR_HXX

#include "lex_token.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"

class MaxforwardProcessor: public Processor {
protected:
	ILexicon *_lexicon;
	int _max_token_length;
	char *_token;

	MaxforwardProcessor();
	bool _can_process(LexToken *token) 
	{
		return (token->get_attr() == LexToken::attr_unknow);
	}
	void _process(LexToken *token, std::vector<LexToken *> &out);

public:
	MaxforwardProcessor(IConfig *config);
	~MaxforwardProcessor();
};

#endif // MAXFORWARD_PROCESSOR_HXX
