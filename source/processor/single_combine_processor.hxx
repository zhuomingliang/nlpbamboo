#ifndef SINGLE_COMBINE_PROCESSOR_HXX
#define SINGLE_COMBINE_PROCESSOR_HXX

#include "lex_token.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"

class SingleCombineProcessor: public Processor {
protected:
	ILexicon *_lexicon;
	int _max_token_length;
	char *_combine;

	SingleCombineProcessor();
	bool _can_process(LexToken *token) {}
	void _process(LexToken *token, std::vector<LexToken *> &out) {};

public:
	void process(std::vector<LexToken *> &in, std::vector<LexToken *> &out);
	SingleCombineProcessor(IConfig *config);
	~SingleCombineProcessor();
};

#endif // SINGLE_COMBINE_PROCESSOR_HXX
