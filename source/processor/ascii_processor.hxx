#ifndef ASCII_PROCESSOR_HXX
#define ASCII_PROCESSOR_HXX

#include "processor.hxx"
#include "ilexicon.hxx"
#include "datrie.hxx"
#include "utf8.hxx"

class AsciiProcessor: public Processor {
protected:
	AsciiProcessor();
	bool _can_process(LexToken *token)
	{
		return (token->get_attr() == LexToken::attr_unknow);
	}
	void _process(LexToken *token, std::vector<LexToken *> &out);
public:
	AsciiProcessor(IConfig *config);
	~AsciiProcessor() {};
};

#endif // ASCII_PROCESSOR_HXX
