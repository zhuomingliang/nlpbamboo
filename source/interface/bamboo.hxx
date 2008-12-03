#ifndef BAMBOO_HXX
#define BAMBOO_HXX

#ifdef __cplusplus

#include "parser_factory.hxx"

#endif

#ifdef __cplusplus
extern "C" {
#endif
void *bamboo_init_ex(const char *parser, const char *cfg);
void bamboo_clean_ex(void *handle);
void *bamboo_init(const char *cfg);
void bamboo_clean(void *handle);
char *bamboo_parse_ex(void *handle);
ssize_t bamboo_parse(void *handle, char **t, const char *s);
const char *bamboo_strerror();
const void *bamboo_getopt(void *handle, enum bamboo_option option);
void bamboo_setopt(void *handle, enum bamboo_option option, void *arg);
#ifdef __cplusplus
}
#endif
#endif
