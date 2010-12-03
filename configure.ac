AC_INIT([nlpbamboo],[1.2.0],[],[bamboo],[http://code.google.com/p/nlpbamboo/])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_CHECK_LIB([crfpp], [crfpp_new], [], [exit 1])
AC_CONFIG_FILES([
     Makefile
     lib/Makefile
     bin/Makefile
])
AC_OUTPUT
