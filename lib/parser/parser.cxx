#include "parser.hxx"

namespace bamboo {
void Parser::setopt(enum bamboo_option option, const void *arg)
{
	switch (option) {
		case BAMBOO_OPTION_TEXT:
			_options["text"] = arg;
			break;
		case BAMBOO_OPTION_TITLE:
			_options["title"] = arg;
			break;
	}
}

const void *Parser::getopt(enum bamboo_option option)
{
	switch (option) {
		case BAMBOO_OPTION_TEXT:
			return _options["text"];
		case BAMBOO_OPTION_TITLE:
			return _options["title"];
	}

	return NULL;
}

};
