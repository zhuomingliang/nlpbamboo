CURDIR := $(SRCTOP)/source/
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

BAMBOO_SRC := processor_factory utf8 token_impl config_finder mmap parser parser_factory custom_parser crf_pos_parser crf_seg_parser ugm_seg_parser mfm_seg_parser crf_ner_nr_parser crf_ner_ns_parser crf_ner_nt_parser crf_ner_np_parser keyword_parser text_parser kea_mmap kea_doc segment_tool token_dict token_aff_dict token_filter ranker prepare_ranker tfidf_ranker graph_ranker udgraph kea_hash kea lexicon trie_debugger double_array datrie simple_config config_factory 

SUBDIR := include kea mmap utf8 trie lexicon config processor common parser

CFLAGS += $(foreach dir,$(SUBDIR), -I$(SRCTOP)/source/$(dir))
CFLAGS += -I$(SRCTOP)/source/include

BAMBOO_CFGDIR=$(INST_ETCDIR)/bamboo/
BAMBOO_DATADIR=$(INST_DATADIR)/bamboo/

CFLAGS += -D"BAMBOO_CFGDIR=\"$(BAMBOO_CFGDIR)\""

$(foreach dir,$(SUBDIR), $(eval -include $(SRCTOP)/source/$(dir)/rule.mk))

$(eval $(call INST_LIB_template,bamboo,libbamboo $(BAMBOO_SRC)))
$(eval $(call INST_SO_template,libbamboo,libbamboo $(BAMBOO_SRC)))

include $(SRCTOP)/source/tools/rule.mk


BTMP=/tmp/.__bamboo.conf.cfg

install ::
	@test -d $(DESTDIR)/$(BAMBOO_CFGDIR) || mkdir -p $(DESTDIR)/$(BAMBOO_CFGDIR)
	@test -d $(DESTDIR)/$(BAMBOO_DATADIR)/template || mkdir -p $(DESTDIR)/$(BAMBOO_DATADIR)/template
	@test -d $(DESTDIR)/$(BAMBOO_DATADIR)/processor || mkdir -p $(DESTDIR)/$(BAMBOO_DATADIR)/processor
	@test -d $(DESTDIR)/$(BAMBOO_DATADIR)/index || mkdir -p $(DESTDIR)/$(BAMBOO_DATADIR)/index
	for file in $(SRCTOP)/etc/*.conf; do \
		sed -e 's,root = /opt/bamboo,root = $(BAMBOO_DATADIR),g' $$file >$(BTMP); \
		install -m 0644 $(BTMP) $(DESTDIR)/$(BAMBOO_CFGDIR)/`basename $${file}`; \
		rm -f $(BTMP); \
    done
	install -m 0644 $(SRCTOP)/template/*.tmpl $(DESTDIR)/$(BAMBOO_DATADIR)/template/
	install -m 0644 $(SRCTOP)/index/*  $(DESTDIR)/$(BAMBOO_DATADIR)/index/




