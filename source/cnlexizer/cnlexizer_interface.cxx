#include <iostream>
#include <stdexcept>

#include "cnlexizer.hxx"
#include "cnlexizer_interface.hxx"

void *cnlexizer_init(const char *cfg)
{
	try {
		return new CNLexizer(cfg);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
		return NULL;
	}
}

void cnlexizer_clean(void *handle)
{
	try {
		if (handle == NULL)
			throw new std::runtime_error("handle is null");
		delete static_cast<CNLexizer *>(handle);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
	}
}

ssize_t cnlexizer_process(void *handle, char *t, const char *s)
{
	try {
		if (handle == NULL)
			throw new std::runtime_error("handle is null");
		if (s == NULL)
			throw new std::runtime_error("input buffer is null");
		if (t == NULL)
			throw new std::runtime_error("output buffer is null");

		if (*s == '\0') {
			*t = '\0';
			return 0;
		}

		CNLexizer *clx = static_cast<CNLexizer *>(handle);
		return clx->process(t, s);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
		return -1;
	}
}
