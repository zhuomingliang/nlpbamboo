#ifndef PHP_BAMBOO_H
#define PHP_BAMBOO_H 1

#define PHP_BAMBOO_VERSION "1.0"
#define PHP_BAMBOO_EXTNAME "bamboo"

PHP_FUNCTION(bamboo_open);
PHP_FUNCTION(bamboo_parse);
PHP_FUNCTION(bamboo_close);

extern zend_module_entry bamboo_module_entry;
#define phpext_bamboo_ptr &bamboo_module_entry

#endif
