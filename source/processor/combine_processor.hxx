#ifndef COMBINE_PROCESSOR_HXX
#define COMBINE_PROCESSOR_HXX

#include "lex_token.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"

class CombineProcessor: public Processor {
protected:
	ILexicon *_lexicon;
	int _max_token_length;
	char *_token;
	char *_combine;

	CombineProcessor();
	bool _can_process(LexToken *token) {}
	void _process(LexToken *token, std::vector<LexToken *> &out) {};

public:
	void process(std::vector<LexToken *> &in, std::vector<LexToken *> &out);
	CombineProcessor(IConfig *config);
	~CombineProcessor();
};

#endif // COMBINE_PROCESSOR_HXX
