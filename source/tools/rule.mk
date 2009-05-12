CURDIR := $(SRCTOP)/source/tools
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)



$(eval $(call EXE_template,bamboo,bamboo,bamboo))
$(eval $(call EXE_template,config,config,config))
$(eval $(call EXE_template,lexicon,lexicon,lexicon trie))
$(eval $(call EXE_template,word_train,word_train,bamboo))
$(eval $(call EXE_template,word_aff_train,word_aff_train,bamboo))
bamboo_LDFLAGS += -ldl
word_train_LDFLAGS += -ldl
word_aff_train_LDFLAGS += -ldl

ifeq ($(CRF_INC_FLAG),yes)
$(eval $(call EXE_template,crf2_tool,crf2_tool,utf8))
$(eval $(call EXE_template,ner_nr_tool,ner_nr_tool,utf8))
$(eval $(call EXE_template,ner_tool,ner_tool,utf8))
endif

#	build_tool(crf2_tool utf8_static)
#	build_tool(ner_nr_tool utf8_static)
#	build_tool(ner_tool utf8_static)

# $(eval $(call SO_template,mod_cnqp_db,mod_cnqp_db))
# $(eval $(call EXE_template,aprtest,aprtest))



