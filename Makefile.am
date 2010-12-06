SUBDIRS = lib bin

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIRS = doc lib test etc template exts

doc_DATA = README ChangeLog AUTHORS COPYING INSTALL

dist-hook:
	for subdir in $(EXTRA_DIRS); do \
	  cp -rp $(top_srcdir)/$$subdir $(distdir); \
	done \
    find $(distdir) -name .svn  | xargs rm -fr;

check_PROGRAMS = utf8_test
utf8_test_SOURCES = test/utf8_test.cxx
utf8_test_LDADD = lib/libbamboo.la

TESTS = utf8_test

BUILD_DIRS = etc template exts 

all-local:
	if [ $(top_srcdir) != $(top_builddir) ]; then \
		for subdir in $(BUILD_DIRS); do \
		  cp -rpu $(top_srcdir)/$$subdir $(top_builddir); \
		done \
	fi
