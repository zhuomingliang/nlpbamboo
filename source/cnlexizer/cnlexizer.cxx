#include <ctype.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "cnlexizer.hxx"
#include "unigram_processor.hxx"

const char CNLexizer::_stream_name_prefix[] = "streamline_";

CNLexizer::CNLexizer(const char *file)
:_config(NULL)
{
	const char *s;
	char *text, *token, delim[] = ",";
	size_t length;

	_config = ConfigFactory::create("lua_config", file);
	_config->get_value("streamline", s);
	length = strlen(s) + strlen(_stream_name_prefix) + 1;
	text = new char[length];
	snprintf(text, length, "%s%s", _stream_name_prefix, s);
	_config->get_value(text, s);
	delete []text;
	text = new char[strlen(s) + 1];
	strcpy(text, s);
	for (token = strtok(text, delim); token; token = strtok(NULL, delim)) {
		_streamline.push_back(new std::string(token));
	}
	_processors["unigram"] = new UnigramProcessor(_config);
}

size_t CNLexizer::process(char *t, const char *s)
{
	std::vector<std::string *>::iterator it;

	_result.erase();
	_prod.clear();
	_prod.push_back(std::pair<std::string, LexAttribute>(s, NULL));
	for (it = _streamline.begin(); it < _streamline.end(); it++) {
		if (_processors[**it])
			_processors[**it]->process(_prod);
		else
			std::cerr << "Invalid module name: " << **it << std::endl;
	}

	std::vector< std::pair<std::string, LexAttribute> >::iterator j;
	for (j = _prod.begin(); j < _prod.end(); j++) 
		_result += j->first + " ";
	strcpy(t, _result.c_str());
	return _result.length();
}

