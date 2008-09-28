#include <ctype.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <dlfcn.h>

#include "cnlexizer.hxx"
#include "prepare_processor.hxx"
#include "ascii_processor.hxx"
#include "maxforward_processor.hxx"
#include "unigram_processor.hxx"
#include "crf_processor.hxx"
#include "crf2_processor.hxx"
#include "combine_processor.hxx"
#include "single_combine_processor.hxx"

CNLexizer::CNLexizer(const char *file)
:_config(NULL), _in(&_lex_token[0]), _out(&_lex_token[1])
{
	std::vector<std::string>::iterator it;

	_config = ConfigFactory::create("simple_config", file);
	_config->get_value("process_chain", _process_chain);
	_config->get_value("libroot", _libroot);

	for (it = _process_chain.begin(); it != _process_chain.end(); it++) {
		std::string libpath;
		_create_t create;
		void *handle = NULL;
		Processor *processor = NULL;

		libpath.append(_libroot).append("/libclxmod_").append(*it).append(".so");
		std::cerr << "Loading Module " << libpath << std::endl;
		handle = dlopen(libpath.c_str(), RTLD_NOW);
		if (!handle)
			throw std::runtime_error(std::string(dlerror()));
		create = (_create_t)dlsym(handle, "create");
		if (!create)
			throw std::runtime_error(std::string(dlerror()));
		processor = create(_config);
		if (!processor)
			throw std::runtime_error(std::string("can not create processor: ") + *it);
		_processors.push_back(processor);
		_handles[*it] = handle;
	}
}

CNLexizer::~CNLexizer()
{
	std::vector<Processor *>::iterator i;
	std::map<std::string, void *>::iterator j;

	for (i = _processors.begin(); i != _processors.end(); i++)
		delete *i;

	for (j = _handles.begin(); j != _handles.end(); j++)
		dlclose(j->second);

	delete _config;
}

size_t CNLexizer::process(char *t, const char *s)
{
	char *neo, *p = t;
	size_t i, length;

	*t = '\0';
	length = utf8::length(s);
	_in->clear();
	if (length > _in->capacity()) {
		_in->reserve(length);
		_out->reserve(length);
	}
	_in->push_back(new LexToken(s));
	length = _processors.size();
	for (i = 0; i < length; i++) {
		_out->clear();
		_processors[i]->process(*_in, *_out);
		/* switch in & out queue */
		_swap = _out;
		_out = _in;
		_in = _swap;
	}

	length = _in->size();
	for (i = 0; i < length; i++) {
		strcpy(p, (*_in)[i]->get_orig_token());
		p += (*_in)[i]->get_orig_bytes();
		*(p++) = ' ';
		*p = '\0';
		delete (*_in)[i];
	}
	return p - t;
}

