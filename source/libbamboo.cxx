#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <stdio.h>

#include "bamboo.hxx"

#define ERROR_BUFFER_SIZE 1024
#define set_error(F,...) snprintf(error_buffer, ERROR_BUFFER_SIZE, "%s: ", __VA_ARGS__)

static char error_buffer[ERROR_BUFFER_SIZE];


const char *bamboo_strerror()
{
	return error_buffer;
}

void *bamboo_init(const char *parser, const char *cfg)
{
	bamboo::ParserFactory *factory;
	try {
		if (parser == NULL)
			throw std::runtime_error("invalid parameters");

		factory = bamboo::ParserFactory::get_instance();
		return factory->create(parser);
	} catch(std::exception &e) {
		set_error("%s", e.what());
		return NULL;
	}
}

void bamboo_setopt(void *handle, enum bamboo_option option, void *arg)
{
	bamboo::Parser *parser = static_cast<bamboo::Parser *>(handle);
	parser->setopt(option, arg);
}

const void *bamboo_getopt(void *handle, enum bamboo_option option)
{
	bamboo::Parser *parser = static_cast<bamboo::Parser *>(handle);
	return parser->getopt(option);
}

char *bamboo_parse(void *handle)
{
	std::vector<bamboo::Token *>				vec;
	std::vector<bamboo::Token *>::iterator		it;
	char			 							*t;
	char			 							*p;

	try {
		if (handle == NULL)
			throw std::runtime_error("invalid parameters");
		
		bamboo::Parser *parser = static_cast<bamboo::Parser *>(handle);
		parser->parse(vec);

		size_t size = (strlen((const char *)parser->getopt(BAMBOO_OPTION_TEXT)) + 1) << 1;
		t = (char *)malloc(size + 1);
		p = t;
		*p = '\0';

		for (it = vec.begin(); it < vec.end(); ++it) {
			const char 		*token = (*it)->get_orig_token();
			ssize_t			len = strlen(token);
			unsigned short	pos = (*it)->get_pos();

			if (pos) len += sizeof(unsigned short) + 1;

			if (p - t <= len) {
				/* expand */
				char *old = t;
				while (size + t < p + len)
					size <<= 1;
				t = (char *)realloc(t, size);
				p = t + (p - old);
			}

			strcpy(p, (*it)->get_orig_token());
			p += strlen((*it)->get_orig_token());
			if (pos) {
				const char *ch = (const char *)&pos;
				*(p++) = ' ';
				if (*(ch + 1)) *(p++) = *(ch + 1);
				if (*ch) *(p++) = *ch;
			}
			*(p++) = ' ';
			delete *it;
		}

		*p = '\0';
		
		return t;

	} catch(std::exception &e) {
		set_error("%s", e.what());
		return NULL;
	}
}

void bamboo_clean(void *handle)
{
	delete static_cast<bamboo::Parser *>(handle);
}

