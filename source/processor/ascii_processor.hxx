#ifndef ASCII_PROCESSOR_HXX
#define ASCII_PROCESSOR_HXX

#include "processor.hxx"
#include "ilexicon.hxx"
#include "datrie.hxx"
#include "utf8.hxx"

class AsciiProcessor: public Processor {
protected:
	DATrie _chinese_number_end;
	DATrie _chinese_punctuation;
	DATrie _chinese_number;
	AsciiProcessor();
	bool _can_process(LexToken *token)
	{
		return (token->get_attr() == LexToken::attr_unknow);
	}
	void _process(LexToken *token, std::vector<LexToken *> &out);
	void _load_lexicon(DATrie &trie, const char *s)
	{
		size_t length, i;
		char token[8];

		length = utf8::length(s);
		for (i = 0; i < length; i++) {
			utf8::sub(token, s, i, 1);
			trie.insert(token, 1);
		}
	}
public:
	AsciiProcessor(IConfig *config);
	~AsciiProcessor() {};
};

#endif // ASCII_PROCESSOR_HXX
