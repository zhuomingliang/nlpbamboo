#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "lexicon_factory.hxx"
#include "crf_processor.hxx"
#include "utf8.hxx"

PROCESSOR_MAGIC
PROCESSOR_MODULE(CRFProcessor)

CRFProcessor::CRFProcessor(IConfig *config)
	:_token(NULL), _result(NULL), _result_top(NULL)
{
	const char *s;
	struct stat buf;

	_token = new char[8];
	 _result = (char *)realloc(_result, 4096);
	 _result_size = 4096;
	config->get_value("crf_model", s);
	_model_parameter.append("-m ").append(s);
	if (stat(s, &buf) == 0) {
		_tagger = CRFPP::createTagger(_model_parameter.c_str());
	} else {
		throw std::runtime_error(std::string("can not load model ") + s + ": " + strerror(errno));
	}
#ifdef TIMING
	_timing_tagger = 0;
	_timing_parser = 0;
	_timing_insert = 0;
#endif
}

CRFProcessor::~CRFProcessor()
{
	delete []_token;
	delete _tagger;
#ifdef TIMING
	std::cerr << "crf1 tagger consume: " << _timing_tagger << "ms" << std::endl;
	std::cerr << "crf1 parser consume: " << _timing_parser << "ms" << std::endl;
	std::cerr << "crf1 insert consume: " << _timing_insert << "ms" << std::endl;
#endif
}

void CRFProcessor::_process(LexToken *token, std::vector<LexToken *> &out)
{
	size_t length, i, j;
	const char *s;
	char tag;
#ifdef TIMING	
	struct timeval tv[2];
	struct timezone tz;
#endif

	_tagger->clear();
	s = token->get_token();
	length = token->get_length();
	if (length > _result_size) {
		_result_size = (length > _result_size << 1)?length + 1:_result_size << 1;
		_result = (char *)realloc(_result, _result_size);
	}
	for (i = 0; i < length; i++) {
		utf8::sub(_token, s, i, 1);
#ifdef TIMING		
		gettimeofday(&tv[0], &tz);
#endif
		_tagger->add(_token);
#ifdef TIMING
		gettimeofday(&tv[1], &tz);
		_timing_tagger += (tv[1].tv_sec - tv[0].tv_sec) * 1000 + (tv[1].tv_usec - tv[0].tv_usec) / 1000;
#endif
	}

#ifdef TIMING		
		gettimeofday(&tv[0], &tz);
#endif
	if (!_tagger->parse()) throw std::runtime_error("crf parse failed!");
#ifdef TIMING
		gettimeofday(&tv[1], &tz);
		_timing_parser += (tv[1].tv_sec - tv[0].tv_sec) * 1000 + (tv[1].tv_usec - tv[0].tv_usec) / 1000;
#endif

#ifdef TIMING		
		gettimeofday(&tv[0], &tz);
#endif
	_result_top = _result;
	for (i = 0; i < _tagger->size(); ++i) {
		s = _tagger->x(i, 0);
		while ( (*(_result_top++) = *(s++)) != 0)
			;
		*(_result_top++) = '\0';
		tag = *_tagger->y2(i);
		if (tag == 'S' || tag == 'E') {
			out.push_back(new LexToken(_result, LexToken::attr_cword));
			_result_top = _result;
		}
	}
#ifdef TIMING
		gettimeofday(&tv[1], &tz);
		_timing_insert += (tv[1].tv_sec - tv[0].tv_sec) * 1000 + (tv[1].tv_usec - tv[0].tv_usec) / 1000;
#endif

}
