#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_bamboo.h"

static function_entry bamboo_functions[] = {
    PHP_FE(bamboo_open, NULL)
    PHP_FE(bamboo_parse, NULL)
    PHP_FE(bamboo_close, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry bamboo_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_BAMBOO_EXTNAME,
    bamboo_functions,
    NULL,
    NULL,
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

PHP_FUNCTION(bamboo_open)
{
    RETURN_STRING("bamboo_open", 1);
}

PHP_FUNCTION(bamboo_parese)
{
    RETURN_STRING("bamboo_parse", 1);
}

PHP_FUNCTION(bamboo_close)
{
    RETURN_STRING("bamboo_close", 1);
}
