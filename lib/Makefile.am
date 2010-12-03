AM_CPPFLAGS = -I$(top_srcdir)/lib/include -I$(top_srcdir)/lib/common -I$(top_srcdir)/lib/config -I$(top_srcdir)/lib/kea -I$(top_srcdir)/lib/lexicon -I$(top_srcdir)/lib/mmap -I$(top_srcdir)/lib/parser -I$(top_srcdir)/lib/processor -I$(top_srcdir)/lib/trie -I$(top_srcdir)/lib/utf8 $(CPPFLAGS)

lib_LTLIBRARIES = libbamboo.la
libbamboo_la_SOURCES = \
					   libbamboo.cxx\
					   utf8/utf8.cxx\
					   mmap/mmap.cxx\
					   trie/datrie.cxx\
					   trie/double_array.cxx\
					   lexicon/liblexicon.cxx\
					   config/simple_config.cxx\
					   config/config_factory.cxx\
					   kea/text_parser.cxx\
					   kea/kea_mmap.cxx\
					   kea/kea_doc.cxx\
					   kea/segment_tool.cxx\
					   kea/token_dict.cxx\
					   kea/token_aff_dict.cxx\
					   kea/token_filter.cxx\
					   kea/ranker.cxx\
					   kea/prepare_ranker.cxx\
					   kea/tfidf_ranker.cxx\
					   kea/graph_ranker.cxx\
					   kea/udgraph.cxx\
					   kea/kea_hash.cxx\
					   kea/kea.cxx\
					   common/token_impl.cxx\
					   common/config_finder.cxx\
					   parser/crf_ner_np_parser.cxx\
					   parser/crf_ner_nr_parser.cxx\
					   parser/crf_ner_ns_parser.cxx\
					   parser/crf_ner_nt_parser.cxx\
					   parser/crf_pos_parser.cxx\
					   parser/crf_seg_parser.cxx\
					   parser/custom_parser.cxx\
					   parser/keyword_parser.cxx\
					   parser/mfm_seg_parser.cxx\
					   parser/parser.cxx\
					   parser/parser_factory.cxx\
					   parser/ugm_seg_parser.cxx\
					   processor/break_processor.cxx\
					   processor/crf_ner_np_processor.cxx\
					   processor/crf_ner_nr_processor.cxx\
					   processor/crf_ner_ns_processor.cxx\
					   processor/crf_ner_nt_processor.cxx\
					   processor/crf_pos_processor.cxx\
					   processor/crf_seg4ner_processor.cxx\
					   processor/crf_seg_processor.cxx\
					   processor/maxforward_combine_processor.cxx\
					   processor/maxforward_processor.cxx\
					   processor/prepare_processor.cxx\
					   processor/processor.cxx\
					   processor/processor_factory.cxx\
					   processor/single_combine_processor.cxx\
					   processor/ugm_seg_processor.cxx

pkginclude_HEADERS = \
				  include/bamboo.hxx\
				  include/bamboo_defs.h\
				  parser/parser.hxx\
				  parser/parser_factory.hxx\
				  common/token.hxx
