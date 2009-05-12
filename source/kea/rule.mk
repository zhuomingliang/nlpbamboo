CURDIR := $(SRCTOP)/source/kea
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

KEA_SRC := text_parser kea_mmap kea_doc segment_tool token_dict token_aff_dict token_filter ranker prepare_ranker tfidf_ranker graph_ranker udgraph kea_hash kea 


$(eval $(call LIB_template,kea,$(KEA_SRC)))

# $(eval $(call SO_template,mod_cnqp_db,mod_cnqp_db))
# $(eval $(call EXE_template,aprtest,aprtest))



