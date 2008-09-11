#ifndef CRF_PROCESSOR_HXX
#define CRF_PROCESSOR_HXX

#include "lex_token.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"
#include <sstream>
#include <crfpp.h>

class CRFProcessor: public Processor {
protected:
	CRFPP::Tagger *_tagger;
	char *_token;
	const char *_ending_tags;
	std::string _result;

	CRFProcessor();
	bool _can_process(LexToken *token) 
	{
		return (token->get_attr() == LexToken::attr_unknow);
	}
	void _process(LexToken *token, std::vector<LexToken *> &out);

public:
	CRFProcessor(IConfig *config);
	~CRFProcessor();
};

#endif // CRF_PROCESSOR_HXX
