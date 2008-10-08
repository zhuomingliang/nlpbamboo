PHP_ARG_ENABLE(bamboo, whether to enable bamboo nlp support,
[ --enable-bamboo   Enable bamboo nlp support])

if test "$PHP_BAMBOO" = "yes"; then
  AC_DEFINE(HAVE_BAMBOO, 1, [Whether you have bamboo])
  PHP_NEW_EXTENSION(bamboo, bamboo.c, $ext_shared)
fi
