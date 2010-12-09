SUBDIRS = lib bin

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIRS = doc lib test etc template exts

docdir = $(prefix)/share/doc/@PACKAGE@
doc_DATA = README AUTHORS COPYING INSTALL ChangeLog

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

all-local: copy_build_dirs etc_config_files

.PHONY: copy_build_dirs
copy_build_dirs:
	if [ $(top_srcdir) != $(top_builddir) ]; then \
		rsync -avC $(top_srcdir)/bin $(top_builddir); \
		for subdir in $(BUILD_DIRS); do \
		  rsync -avC --delete $(top_srcdir)/$$subdir $(top_builddir); \
		done \
	fi

.PHONY: etc_config_files
etc_config_files: $(top_builddir)/etc/bamboo.cfg $(top_builddir)/etc/crf_ner_np.conf $(top_builddir)/etc/crf_ner_nr.conf $(top_builddir)/etc/crf_ner_ns.conf $(top_builddir)/etc/crf_ner_nt.conf $(top_builddir)/etc/crf_pos.conf $(top_builddir)/etc/crf_seg.conf $(top_builddir)/etc/keyword.conf $(top_builddir)/etc/mfm_seg.conf $(top_builddir)/etc/ugm_seg.conf

%: %.in
	sed -e 's,@BAMBOO_ROOT@,$(abs_top_builddir),g' < $^ > $@
