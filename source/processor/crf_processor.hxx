#ifndef CRF_PROCESSOR_HXX
#define CRF_PROCESSOR_HXX

#include "lex_token.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"
#include <sstream>
#include <crfpp.h>

#define TIMING

class CRFProcessor: public Processor {
protected:
	CRFPP::Tagger *_tagger;
	size_t _result_size;
	char *_token, *_result, *_result_top;
	std::string _model_parameter;
	
	CRFProcessor();
	bool _can_process(LexToken *token) 
	{
		return (token->get_attr() == LexToken::attr_unknow);
	}
	void _process(LexToken *token, std::vector<LexToken *> &out);
#ifdef TIMING
	int _timing_parser, _timing_tagger, _timing_insert;
#endif
public:
	CRFProcessor(IConfig *config);
	~CRFProcessor();
};

#endif // CRF_PROCESSOR_HXX
