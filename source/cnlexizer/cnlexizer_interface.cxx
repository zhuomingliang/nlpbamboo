#include "cnlexizer.hxx"
#include "stdio.h"
#include <stdexcept>
#include "cnlexizer_interface.hxx"

void *cnlexizer_init(const char *cfg)
{
	CNLexizer *handle;
	try {
		if (cfg == NULL)
			throw new std::runtime_error("empty cfg filename");
		handle = new CNLexizer(cfg);
		return handle;
	} catch(std::exception &e) {
		fprintf(stderr, "ERROR: %s\n", e.what());
		return NULL;
	}
}

void cnlexizer_clean(void *handle)
{
	if (handle) delete (CNLexizer *)handle;
}

size_t cnlexizer_process(void *handle, char *t, const char *s)
{
	size_t length = 0;
	try {
		if (handle == NULL)
			throw new std::runtime_error("invalid sgmt handle");
		if (s == NULL)
			throw new std::runtime_error("empty input string");
		if (t == NULL)
			throw new std::runtime_error("empty buffer");

		CNLexizer *clx = (CNLexizer *)handle;
		length = clx->process(t, s);
		return length;
	} catch(std::exception &e) {
		fprintf(stderr, "ERROR: %s\n", e.what());
		return 0;
	}
}
