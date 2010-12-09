AM_CPPFLAGS = -I$(top_srcdir)/lib/include -I$(top_srcdir)/lib/common -I$(top_srcdir)/lib/config -I$(top_srcdir)/lib/kea -I$(top_srcdir)/lib/lexicon -I$(top_srcdir)/lib/mmap -I$(top_srcdir)/lib/parser -I$(top_srcdir)/lib/processor -I$(top_srcdir)/lib/trie -I$(top_srcdir)/lib/utf8 $(CPPFLAGS)

LDADD = ../lib/libbamboo.la

noinst_PROGRAMS = \
			   bamboo\
			   config\
			   crf2_tool\
			   lexicon\
			   ner_nr_tool\
			   ner_tool\
			   word_aff_train\
			   word_train

config_SOURCES = config.cxx
crf2_tool_SOURCES = crf2_tool.cxx
lexicon_SOURCES = lexicon.cxx
ner_nr_tool_SOURCES = ner_nr_tool.cxx
ner_tool_SOURCES = ner_tool.cxx
word_aff_train_SOURCES = word_aff_train.cxx
word_train_SOURCES = word_train.cxx
