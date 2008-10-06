#ifndef SINGLE_COMBINE_PROCESSOR_HXX
#define SINGLE_COMBINE_PROCESSOR_HXX

#include "lex_token.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"
#include "utf8.hxx"

class SingleCombineProcessor: public Processor {
protected:
	ILexicon *_lexicon_combine, *_lexicon_number_trailing;
	std::string _combine;
	SingleCombineProcessor();
	int _combine_double_same_character;
	bool _can_process(LexToken *token) {}
	void _process(LexToken *token, std::vector<LexToken *> &out) {};
	void _make_combine(std::vector<LexToken *> &in, int i, int with)
	{
		/* with: a bitmap indicates copy range. 
		 * binary  :  0        0      0
		 * position:  ^(i-1)   ^(i)   ^(i + 1)
		 * */
		_combine.erase();
		if (with & 4) _combine.append(in[i - 1]->get_orig_token());
		if (with & 2) _combine.append(in[i]->get_orig_token());
		if (with & 1) _combine.append(in[i + 1]->get_orig_token());
	}

public:
	void process(std::vector<LexToken *> &in, std::vector<LexToken *> &out);
	SingleCombineProcessor(IConfig *config);
	~SingleCombineProcessor();
};

#endif // SINGLE_COMBINE_PROCESSOR_HXX
