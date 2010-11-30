PHP_ARG_WITH(bamboo, for bamboo support,
[  --with-bamboos         Where to find bamboo headers and libs])

for i in $PHP_BAMBOO/include/bamboo /usr/include/bamboo /usr/local/include/bamboo include/bamboo; do
	if test -r "$i/bamboo.hxx"; then
		BAMBOO_INCLUDE=$i
		AC_DEFINE(HAVE_BAMBOO_HXX,1,[Whether to have bamboo.hxx])
	fi
done 
if test -z "$BAMBOO_INCLUDE"; then
	AC_MSG_ERROR(Can not find bamboo.hxx. Please specify correct Bamboo installation path)
fi

for i in $PHP_BAMBOO/lib /usr/lib /usr/local/lib /usr/lib64 /usr/local/lib64; do
	if test -r "$i/libbamboo.so"; then
		BAMBOO_LIBRARY=$i
		AC_DEFINE(HAVE_LIBBAMBOO,1,[Whether to have libbamboo])
	fi
done 
if test -z "$BAMBOO_LIBRARY"; then
	AC_MSG_ERROR(Can not find libbamboo.so.2. Please specify correct Bamboo installation path)
fi

AC_CHECK_LIB(bamboo, bamboo_parse, AC_DEFINE(HAVE_LIBBAMBOO, 1, [libbamboo]))

PHP_ADD_INCLUDE($BAMBOO_INCLUDE)
PHP_ADD_LIBRARY_WITH_PATH(bamboo, $BAMBOO_LIBRARY, BAMBOO_SHARED_LIBADD)
PHP_SUBST(BAMBOO_SHARED_LIBADD)
PHP_NEW_EXTENSION(bamboo, bamboo.c, $ext_shared)
