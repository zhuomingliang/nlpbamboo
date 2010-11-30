ACLOCAL_AMFLAGS = -I m4

rootdir = /opt/$(PACKAGE)

nobase_root_DATA = \
				   etc/build_settings\
				   etc/bamboo.cfg\
				   etc/crf_pos.conf\
				   etc/crf_ner_nr.conf\
				   etc/crf_ner_ns.conf\
				   etc/crf_ner_nt.conf\
				   etc/crf_ner_np.conf\
				   etc/keyword.conf\
				   etc/mfm_seg.conf\
				   etc/ugm_seg.conf\
				   template/crf_seg.tmpl\
				   template/crf_pos.tmpl\
				   template/crf_ner_nr.tmpl\
				   template/crf_ner_ns.tmpl\
				   template/crf_ner_nt.tmpl\
				   exts/php/bamboo.c\
				   exts/php/config.m4\
				   exts/php/php_bamboo.h\
				   exts/php/sample.php\
				   exts/postgres/chinese_parser/chinese_parser.c\
				   exts/postgres/chinese_parser/chinese_parser.sql.in\
				   exts/postgres/chinese_parser/Makefile\
				   exts/postgres/chinese_parser/uninstall_chinese_parser.sql\
				   exts/postgres/pg_keyword/Makefile\
				   exts/postgres/pg_keyword/pg_keyword.c\
				   exts/postgres/pg_keyword/pg_keyword.sql.in\
				   exts/postgres/pg_keyword/uninstall_pg_keyword.sql\
				   exts/postgres/pg_tokenize/Makefile\
				   exts/postgres/pg_tokenize/pg_tokenize.c\
				   exts/postgres/pg_tokenize/pg_tokenize.sql.in\
				   exts/postgres/pg_tokenize/uninstall_pg_tokenize.sql\
				   exts/postgres/pg_tokenize/pg_tokenize.so

EXTRA_DIRS = doc source test etc template exts

dist-hook:
	for subdir in $(EXTRA_DIRS); do \
	  cp -rp $(top_srcdir)/$$subdir $(distdir); \
	  rm -f $(distdir)/$$subdir/*~; \
	  rm -f $(distdir)/$$subdir/*.{bak,orig}; \
	  rm -rf $(distdir)/$$subdir/CVS; \
	  rm -rf $(distdir)/$$subdir/.svn; \
	  rm -rf $(distdir)/.svn; \
	  rm -rf $(distdir)/*/.svn; \
	  rm -rf $(distdir)/*/*/.svn; \
	  rm -rf $(distdir)/$$subdir/*/CVS; \
	  rm -rf $(distdir)/$$subdir/*/.svn; \
	  find $(distdir) -name .svn  | xargs rm -fr; \
	done

AM_CPPFLAGS = -I$(top_srcdir)/source/include -I$(top_srcdir)/source/common -I$(top_srcdir)/source/config -I$(top_srcdir)/source/kea -I$(top_srcdir)/source/lexicon -I$(top_srcdir)/source/mmap -I$(top_srcdir)/source/parser -I$(top_srcdir)/source/processor -I$(top_srcdir)/source/trie -I$(top_srcdir)/source/utf8 $(CPPFLAGS)

lib_LTLIBRARIES = libbamboo.la
libbamboo_la_SOURCES = \
					   source/libbamboo.cxx\
					   source/utf8/utf8.cxx\
					   source/mmap/mmap.cxx\
					   source/trie/datrie.cxx\
					   source/trie/double_array.cxx\
					   source/lexicon/liblexicon.cxx\
					   source/config/simple_config.cxx\
					   source/config/config_factory.cxx\
					   source/kea/text_parser.cxx\
					   source/kea/kea_mmap.cxx\
					   source/kea/kea_doc.cxx\
					   source/kea/segment_tool.cxx\
					   source/kea/token_dict.cxx\
					   source/kea/token_aff_dict.cxx\
					   source/kea/token_filter.cxx\
					   source/kea/ranker.cxx\
					   source/kea/prepare_ranker.cxx\
					   source/kea/tfidf_ranker.cxx\
					   source/kea/graph_ranker.cxx\
					   source/kea/udgraph.cxx\
					   source/kea/kea_hash.cxx\
					   source/kea/kea.cxx\
					   source/common/token_impl.cxx\
					   source/common/config_finder.cxx\
					   source/parser/crf_ner_np_parser.cxx\
					   source/parser/crf_ner_nr_parser.cxx\
					   source/parser/crf_ner_ns_parser.cxx\
					   source/parser/crf_ner_nt_parser.cxx\
					   source/parser/crf_pos_parser.cxx\
					   source/parser/crf_seg_parser.cxx\
					   source/parser/custom_parser.cxx\
					   source/parser/keyword_parser.cxx\
					   source/parser/mfm_seg_parser.cxx\
					   source/parser/parser.cxx\
					   source/parser/parser_factory.cxx\
					   source/parser/ugm_seg_parser.cxx\
					   source/processor/break_processor.cxx\
					   source/processor/crf_ner_np_processor.cxx\
					   source/processor/crf_ner_nr_processor.cxx\
					   source/processor/crf_ner_ns_processor.cxx\
					   source/processor/crf_ner_nt_processor.cxx\
					   source/processor/crf_pos_processor.cxx\
					   source/processor/crf_seg4ner_processor.cxx\
					   source/processor/crf_seg_processor.cxx\
					   source/processor/maxforward_combine_processor.cxx\
					   source/processor/maxforward_processor.cxx\
					   source/processor/prepare_processor.cxx\
					   source/processor/processor.cxx\
					   source/processor/processor_factory.cxx\
					   source/processor/single_combine_processor.cxx\
					   source/processor/ugm_seg_processor.cxx

#libbamboo_la_CPPFLAGS = $(AM_CPPFLAGS)
libbamboo_la_LDFLAGS = -version-info 3:0:0

pkginclude_HEADERS = \
				  source/include/bamboo.hxx\
				  source/include/bamboo_defs.h\
				  source/parser/parser.hxx\
				  source/parser/parser_factory.hxx\
				  source/common/token.hxx

bindir = $(rootdir)/bin
bin_SCRIPTS = \
				   source/tools/auto_build\
				   source/tools/check_build\
				   source/tools/convert_pos\
				   source/tools/crf_tool\
				   source/tools/make_turing_data\
				   source/tools/ngm_tool\
				   source/tools/pdc_normalize\
				   source/tools/pdc_normalize_4nr\
				   source/tools/pdc_pos_tool\
				   source/tools/pdc_tool\
				   source/tools/switch_column\
				   source/tools/update_user_combine

bin_PROGRAMS = \
			   bamboo\
			   config\
			   crf2_tool\
			   lexicon\
			   ner_nr_tool\
			   ner_tool\
			   word_aff_train\
			   word_train

bamboo_SOURCES = source/tools/bamboo.cxx
bamboo_LDADD = libbamboo.la

config_SOURCES = source/tools/config.cxx
config_LDADD = libbamboo.la

crf2_tool_SOURCES = source/tools/crf2_tool.cxx
crf2_tool_LDADD = libbamboo.la

lexicon_SOURCES = source/tools/lexicon.cxx
lexicon_LDADD = libbamboo.la

ner_nr_tool_SOURCES = source/tools/ner_nr_tool.cxx
ner_nr_tool_LDADD = libbamboo.la

ner_tool_SOURCES = source/tools/ner_tool.cxx
ner_tool_LDADD = libbamboo.la

word_aff_train_SOURCES = source/tools/word_aff_train.cxx
word_aff_train_LDADD = libbamboo.la

word_train_SOURCES = source/tools/word_train.cxx
word_train_LDADD = libbamboo.la
