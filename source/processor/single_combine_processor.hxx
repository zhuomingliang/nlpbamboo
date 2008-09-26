#ifndef SINGLE_COMBINE_PROCESSOR_HXX
#define SINGLE_COMBINE_PROCESSOR_HXX

#include "lex_token.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"
#include "utf8.hxx"

class SingleCombineProcessor: public Processor {
protected:
	ILexicon *_lexicon_combine, *_lexicon_number_trailing;
	int _max_token_length;
/*	
	struct {
		char *base, *top;
	} _combine;
*/
	std::string _combine;
	SingleCombineProcessor();
	bool _can_process(LexToken *token) {}
	void _process(LexToken *token, std::vector<LexToken *> &out) {};
	void _load_lexicon(DATrie &trie, const char *s)
	{
		size_t length, i;
		char uch[4];

		length = utf8::length(s);
		for (i = 0; i < length; i++) {
			utf8::sub(uch, s, i, 1);
			trie.insert(uch, i + 1);
		}
	}

public:
	void process(std::vector<LexToken *> &in, std::vector<LexToken *> &out);
	SingleCombineProcessor(IConfig *config);
	~SingleCombineProcessor();
};

#endif // SINGLE_COMBINE_PROCESSOR_HXX
