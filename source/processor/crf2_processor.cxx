#include "lexicon_factory.hxx"
#include "crf2_processor.hxx"
#include "utf8.hxx"
#include <cassert>
#include <cstdio>
#include <stdexcept>

PROCESSOR_MAGIC
PROCESSOR_MODULE(CRF2Processor)

CRF2Processor::CRF2Processor(IConfig *config) {
	const char *s;
	_token = new char[8];
	struct stat st;

	config->get_value("crf_ending_tags", _ending_tags);
	config->get_value("crf2_model", s);

	std::string model;
	model = std::string("-m ") + std::string(s);
	if (stat(s, &st) == 0) {
		_tagger = CRFPP::createTagger(model.c_str());
	} else {
		throw std::runtime_error(std::string("can not load model ") + s + ": " + strerror(errno));
	}
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
	size_t i, offset, size = in.size();

	_tagger->clear();
	for (i = 0; i < size; ++i) {
		LexToken *cur_tok = in[i];
		const char *tok_str = cur_tok->get_token();
		if(cur_tok->get_attr() == LexToken::attr_punct) tok_str = cur_tok->get_orig_token();
		const char *data[] = {
			tok_str,
			_get_crf2_tag(cur_tok->get_attr()),
			NULL
		};
		_tagger->add(2, data);

		if(*tok_str=='!' || *tok_str=='?' || *tok_str==';' || !strcmp(tok_str, "。")) {
			offset = i - _tagger->size() + 1;
			_crf2_tagger(in, offset, out);
			_tagger->clear();
		}
	}
	offset = i - _tagger->size();
	_crf2_tagger(in, offset, out);
}

void CRF2Processor::_crf2_tagger(std::vector<LexToken *> &in, size_t offset, std::vector<LexToken *> &out) {
	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");

	_result.clear();
	_result_orig.clear();

	for (size_t i = 0; i < _tagger->size(); ++i) {
		_result.append(_tagger->x(i, 0));
		LexToken *cur_tok = in[offset+i];
		_result_orig.append(cur_tok->get_orig_token());
		const char * tag = _tagger->y2(i);
		int attr = cur_tok->get_attr();
		if(attr==LexToken::attr_unknow) attr = LexToken::attr_cword;
		if(attr==LexToken::attr_alpha || attr==LexToken::attr_number || attr==LexToken::attr_punct)	tag = "S";
		if (*tag=='S' || *tag=='E') {
			out.push_back(new LexToken(_result.c_str(), _result_orig.c_str(), attr));
			_result.clear();
			_result_orig.clear();
		}
		delete cur_tok;
	}

	_tagger->clear();
}

