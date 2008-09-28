#ifndef CNLEXIZER_HXX
#define CNLEXIZER_HXX

#include <stdexcept>
#include <cstring>
#include <vector>

#include "lexicon_factory.hxx"
#include "config_factory.hxx"
#include "processor.hxx"
#include "lex_token.hxx"

class CNLexizer {
private:
	
public:
	size_t process(char *t, const char *s);
	CNLexizer(const char *file);
	~CNLexizer();
protected:
	IConfig *_config;
	const char *processor_root;
	std::vector<std::string> _process_chain;
	std::vector<LexToken *> _token_fifo[2];
	std::vector<LexToken *> *_in, *_out, *_swap;
	std::vector<Processor *> _processors;
	std::vector<void *> _dl_handles;
	typedef Processor* (*_create_processor_t)(IConfig *);
	void _lazy_create_config(const char *);
#ifdef TIMING
	size_t _timing_process[128];
#endif
};

#endif
