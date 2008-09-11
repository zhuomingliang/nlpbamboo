#include "lexicon_factory.hxx"
#include "crf_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <iostream>
#include <cstdio>
#include <stdexcept>

CRFProcessor::CRFProcessor(IConfig *config)
{
	const char *s;

	config->get_value("crf_ending_tags", _ending_tags);
	config->get_value("crf_model", s);
	_tagger = CRFPP::createTagger(s);
	_token = new char[8];
}

CRFProcessor::~CRFProcessor()
{
	delete []_token;
	delete _tagger;
}

void CRFProcessor::_process(LexToken *token, std::vector<LexToken *> &out)
{
	size_t length, i, j;
	char *s;


	_tagger->clear();
	s = token->get_token();
	length = utf8::length(s);
	for (i = 0; i < length; i++) {
		utf8::sub(_token, s, i, 1);
		_tagger->add(_token);
	}
	
	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");

	_result.clear();
	for (i = 0; i < _tagger->size(); ++i) {
		_result.append(_tagger->x(i, 0));
		if (strstr(_ending_tags, _tagger->y2(i))) {
			out.push_back(new LexToken(_result.c_str(), LexToken::attr_cword));
			_result.clear();
		}
	}

}
