#include "lexicon_factory.hxx"
#include "crf2_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <cstdio>
#include <stdexcept>

CRF2Processor::CRF2Processor(IConfig *config) {
	const char *model;

	config->get_value("crf_ending_tags", _ending_tags);
	config->get_value("crf2_model", model);
	_tagger = CRFPP::createTagger(model);
	_token = new char[8];
}

CRF2Processor::~CRF2Processor()
{
	delete []_token;
	delete _tagger;
}

inline const char *CRF2Processor::_get_crf2_tag(int attr) {
	switch(attr) {
	case LexToken::attr_number:
	case LexToken::attr_alpha:
		return "ASCII";
	case LexToken::attr_punct:
		return "PUNC";
	case LexToken::attr_cword:
		return "CN";
	default:
		return "CN";
	}
}

void CRF2Processor::process(std::vector<LexToken *> &in, std::vector<LexToken *> &out) {
	size_t i, size = in.size();

	_tagger->clear();
	for (i = 0; i < size; ++i) {
		LexToken *cur_tok = in[i];
		const char *data[] = {
			cur_tok->get_token(),
			_get_crf2_tag(cur_tok->get_attr()),
			NULL
		};
		_tagger->add(2, data);
	}
	
	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");

	_result.clear();
	for (i = 0; i < _tagger->size(); ++i) {
		_result.append(_tagger->x(i, 0));
		const char * tag = _tagger->y2(i);
		int attr = in[i]->get_attr();
		if(attr==LexToken::attr_alpha || attr==LexToken::attr_number || attr==LexToken::attr_punct)	tag = "S";
		if (strstr(_ending_tags, _tagger->y2(i))) {
			out.push_back(new LexToken(_result.c_str(), LexToken::attr_cword));
			_result.clear();
		}
		delete in[i];
	}
}

