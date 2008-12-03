#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <stdexcept>

#include "bamboo.hxx"

#define ERROR_BUFFER_SIZE 1024
#define set_error(F,...) snprintf(error_buffer, ERROR_BUFFER_SIZE, "%s: ", __VA_ARGS__)

static char error_buffer[ERROR_BUFFER_SIZE];


const char *bamboo_strerror()
{
	return error_buffer;
}

void *bamboo_init_ex(const char *parser, const char *cfg)
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

void *bamboo_init(const char *cfg)
{
	return bamboo_init_ex("custom", cfg);
}

ssize_t bamboo_parse(void *handle, char **t, const char *s)
{
	std::vector<bamboo::Token *>				vec;
	std::vector<bamboo::Token *>::iterator		it;
	char			 							*p;

	try {
		if (handle == NULL || t == NULL || s == NULL)
			throw std::runtime_error("invalid parameters");
		
		size_t size = (strlen(s) + 1) << 1;
		bamboo::Parser *parser = static_cast<bamboo::Parser *>(handle);
		parser->setopt(BAMBOO_OPTION_TEXT, s);
		parser->parse(vec);

		*t = (char *)malloc(size + 1);
		p = *t;
		*p = '\0';

		for (it = vec.begin(); it < vec.end(); ++it) {
			const char 	*token = (*it)->get_orig_token();
			ssize_t		len = strlen(token);

			if (p - *t <= len) {
				/* expand */
				char *old = *t;
				size <<= 1;
				*t = (char *)realloc(*t, size);
				p = *t + (p - old);
			}

			strcpy(p, (*it)->get_orig_token());
			p += strlen((*it)->get_orig_token());
			*(p++) = ' ';
			delete *it;
		}

		*p = '\0';
		
		return 0;

	} catch(std::exception &e) {
		set_error("%s", e.what());
		return -1;
	}
}

void bamboo_clean_ex(void *handle)
{
	delete static_cast<bamboo::Parser *>(handle);
}

void bamboo_clean(void *handle)
{
	bamboo_clean_ex(handle);
}
