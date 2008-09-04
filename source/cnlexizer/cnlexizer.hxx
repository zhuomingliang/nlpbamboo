#ifndef CNLEXIZER_HXX
#define CNLEXIZER_HXX

#include <stdexcept>
#include <cstring>
#include <vector>
#include <map>

#include "lexicon_factory.hxx"
#include "config_factory.hxx"
#include "iprocessor.hxx"
#include "lex_token.hxx"

class CNLexizer {
private:

public:
	size_t process(char *t, const char *s);
	CNLexizer(const char *file);
	~CNLexizer()
	{
		delete _config;
	}

protected:
	IConfig *_config;
	static const char _stream_name_prefix[];
	std::vector<std::string> _streamline;
	std::vector<LexToken *> _lex_token[2];
	std::vector<LexToken *> *_in, *_out, *_swap;
	std::map<std::string, Processor *> _processors;
};

#endif
