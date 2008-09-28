#ifndef CNLEXIZER_HXX
#define CNLEXIZER_HXX

#include <stdexcept>
#include <cstring>
#include <vector>
#include <map>

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
	const char *_libroot;
	std::vector<std::string> _process_chain;
	std::vector<LexToken *> _lex_token[2];
	std::vector<LexToken *> *_in, *_out, *_swap;
	std::vector<Processor *> _processors;
	std::map<std::string, void *> _handles;
	typedef Processor* (*_create_t)(IConfig *);
};

#endif
