#ifndef PARSER_HXX
#define PARSER_HXX

#include <vector>
#include <string>
#include <map>

#include "bamboo_defs.h"
#include "token.hxx"

namespace bamboo {

class Parser {
private:
	void *_handle;
	std::map<std::string, const void *> _options;
public:
	Parser () {};
	Parser (const char *filename);
	virtual void setopt(enum bamboo_option option, const void *arg);
	virtual const void *getopt(enum bamboo_option option);
	virtual int parse(std::vector<Token *> &out)=0;
	virtual ~Parser() {};
};

};

#endif
