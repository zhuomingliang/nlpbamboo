#ifndef PARSER_HXX
#define PARSER_HXX

#include "token.hxx"

namespace bamboo {

class Parser {
private:
	void *_handle;
public:
	Parser () {};
	Parser (const char *filename);
	virtual int parse(std::vector<Token *> &out, const char *s)=0;
	virtual ~Parser() {};
};

};

#endif
