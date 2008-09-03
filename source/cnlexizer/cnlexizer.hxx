#ifndef CNLEXIZER_HXX
#define CNLEXIZER_HXX

#include <stdexcept>
#include <cstring>
#include <vector>
#include <map>

#include "lexicon_factory.hxx"
#include "config_factory.hxx"
#include "iprocessor.hxx"

class CNLexizer {
private:

public:
	size_t process(char *t, const char *s);
	CNLexizer(const char *file);
	~CNLexizer()
	{
		std::vector<std::string *>::iterator it;
		for (it = _streamline.begin(); it < _streamline.end(); it++) {
			delete *it;
		}
		delete _config;
	}

protected:
	IConfig *_config;
	std::string _result;
	static const char _stream_name_prefix[];
	std::vector<std::string *> _streamline;
	std::vector< std::pair<std::string, LexAttribute> > _prod;
	std::map<std::string, IProcessor *> _processors;
};

#endif
