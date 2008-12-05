#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_bamboo.h"

#include "bamboo.hxx"

static function_entry bamboo_functions[] = {
    PHP_FE(bamboo_parse, NULL)
    PHP_FE(bamboo_setopt, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry bamboo_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_BAMBOO_EXTNAME,
    bamboo_functions,
    PHP_MINIT(bamboo),
    PHP_MSHUTDOWN(bamboo),
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_BAMBOO_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_BAMBOO
ZEND_GET_MODULE(bamboo)
#endif


PHP_INI_BEGIN()
PHP_INI_ENTRY("bamboo.parsers", "crf_seg", PHP_INI_ALL, NULL)
PHP_INI_END()


HashTable bamboo_parser_handlers;

PHP_MINIT_FUNCTION(bamboo)
{
	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("BAMBOO_OPTION_TEXT", BAMBOO_OPTION_TEXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BAMBOO_OPTION_TITLE", BAMBOO_OPTION_TITLE, CONST_CS | CONST_PERSISTENT);

	char *p = NULL, *s = estrdup(INI_STR("bamboo.parsers"));

	if (s == NULL) return FAILURE;

	zend_hash_init(&bamboo_parser_handlers, 0, NULL, NULL, 1);
	for (p = strtok(s, ","); p; p = strtok(NULL, ",")) {
		void *h = bamboo_init(p, NULL);
		if (h == NULL) {
			fprintf(stderr, "failed to init parser %s\n", p);
			return FAILURE;
		}
		zend_hash_add(&bamboo_parser_handlers,
			p, strlen(p) + 1, &h, sizeof(void *), NULL);
	}

	efree(s);

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(bamboo)
{
	char *p = NULL, *s = estrdup(INI_STR("bamboo.parsers"));
	void **hnd;
	for (p = strtok(s, ","); p; p = strtok(NULL, ",")) {
		if (zend_hash_find(&bamboo_parser_handlers, p, strlen(p) + 1, (void **) &hnd) == SUCCESS)
		{
			bamboo_clean(*hnd);
		}

	}
	efree(s);
	UNREGISTER_INI_ENTRIES();
	zend_hash_destroy(&bamboo_parser_handlers);
	fprintf(stderr, "shutdown\n");
    return SUCCESS;
}

PHP_FUNCTION(bamboo_setopt)
{
	int name_len, arg_len;
	long opt;
	char *arg, *name;
	void **hnd;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sls",
		&name, &name_len, &opt, &arg, &arg_len) == FAILURE)
		RETURN_NULL();


	if (zend_hash_find(&bamboo_parser_handlers, name, name_len + 1, (void **) &hnd) == FAILURE)
		RETURN_NULL();

	bamboo_setopt(*hnd, opt, estrdup(arg));
	RETURN_LONG(SUCCESS)
}

PHP_FUNCTION(bamboo_parse)
{
	char *s = NULL, *t = NULL, *name = NULL;
	int name_len;
	void **hnd;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE)
		RETURN_NULL();

	if (zend_hash_find(&bamboo_parser_handlers, name, name_len + 1, (void **) &hnd) == FAILURE)
		RETURN_NULL();

	if ((t = bamboo_parse(*hnd)) == NULL)
		RETURN_NULL();

	s = estrdup(t);
	free(t);

	RETURN_STRING(s, 1);
}

