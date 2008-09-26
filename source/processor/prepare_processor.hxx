#ifndef PREPARE_PROCESSOR_HXX
#define PREPARE_PROCESSOR_HXX

#include "processor.hxx"
#include "ilexicon.hxx"
#include "datrie.hxx"
#include "utf8.hxx"

class PrepareProcessor: public Processor {
protected:
	PrepareProcessor();
	bool _can_process(LexToken *token)
	{
		return (token->get_attr() == LexToken::attr_unknow);
	}
	void _process(LexToken *token, std::vector<LexToken *> &out);
public:
	PrepareProcessor(IConfig *config);
	~PrepareProcessor() {};
};

#endif // ASCII_PROCESSOR_HXX
