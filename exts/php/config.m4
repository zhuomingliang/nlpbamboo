PHP_ARG_ENABLE(bamboo, whether to enable bamboo nlp support,
[ --enable-bamboo   Enable bamboo nlp support])

if test "$PHP_BAMBOO" != "no"; then
	for i in /opt/bamboo/include /usr/include /usr/local/include; do
		if test -r "$i/ibamboo.hxx"; then
			BAMBOO_INCLUDE=$i
			AC_DEFINE(HAVE_IBAMBOO_HXX,1,[Whether to have ibamboo.hxx])
		fi
	done 
	if test -z "$BAMBOO_INCLUDE"; then
		AC_MSG_ERROR(Can not find ibamboo.hxx. Please specify correct Bamboo installation path)
	fi

	for i in /opt/bamboo/lib /usr/lib /usr/local/lib; do
		if test -r "$i/libbamboo.so"; then
			BAMBOO_LIBRARY=$i
			AC_DEFINE(HAVE_LIBBAMBOO,1,[Whether to have libbamboo])
		fi
	done 
	if test -z "$BAMBOO_LIBRARY"; then
		AC_MSG_ERROR(Can not find libbamboo.so. Please specify correct Bamboo installation path)
	fi

	AC_CHECK_LIB(bamboo, bamboo_parse, AC_DEFINE(HAVE_LIBBAMBOO, 1, [libbamboo]))

	PHP_ADD_INCLUDE($BAMBOO_INCLUDE)
	PHP_ADD_LIBRARY_WITH_PATH(bamboo, $BAMBOO_LIBRARY, BAMBOO_SHARED_LIBADD)
	PHP_SUBST(BAMBOO_SHARED_LIBADD)
	PHP_NEW_EXTENSION(bamboo, bamboo.c, $ext_shared)
fi
