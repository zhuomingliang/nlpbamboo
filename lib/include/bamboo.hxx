#ifndef BAMBOO_HXX
#define BAMBOO_HXX

#include "bamboo_defs.h"

#ifdef __cplusplus

#include "parser_factory.hxx"

#endif

#ifdef __cplusplus
extern "C" {
#endif
void *bamboo_init(const char *parser, const char *cfg);
void bamboo_clean(void *handle);
char *bamboo_parse(void *handle);
const char *bamboo_strerror();
const void *bamboo_getopt(void *handle, enum bamboo_option option);
void bamboo_setopt(void *handle, enum bamboo_option option, void *arg);
#ifdef __cplusplus
}
#endif
#endif
