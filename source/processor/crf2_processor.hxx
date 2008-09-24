#ifndef CRF2_PROCESSOR_HXX
#define CRF2_PROCESSOR_HXX

#include "lex_token.hxx"
#include "processor.hxx"
#include "ilexicon.hxx"
#include <sstream>
#include <crfpp.h>

class CRF2Processor: public Processor {
protected:
	CRFPP::Tagger *_tagger;
	char *_token;
	const char *_ending_tags;
	std::string _result;

	const char *_get_crf2_tag(int attr);

	CRF2Processor();
	bool _can_process(LexToken *token) {};
	void _process(LexToken *token, std::vector<LexToken *> &out) {};
	void _crf2_tagger(std::vector<LexToken *> &in, std::vector<LexToken *> &out);

public:
	CRF2Processor(IConfig *config);
	~CRF2Processor();

	void process(std::vector<LexToken *> &in, std::vector<LexToken *> &out);
};

#endif // CRF2_PROCESSOR_HXX
