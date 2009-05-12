CURDIR := $(SRCTOP)/source/tools
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)



$(eval $(call INST_EXE_template,bamboo,bamboo,bamboo))
$(eval $(call INST_EXE_template,config,config,config))
$(eval $(call INST_EXE_template,lexicon,lexicon,lexicon trie))
$(eval $(call INST_EXE_template,word_train,word_train,bamboo))
$(eval $(call INST_EXE_template,word_aff_train,word_aff_train,bamboo))
bamboo_LDFLAGS += -ldl
word_train_LDFLAGS += -ldl
word_aff_train_LDFLAGS += -ldl

ifeq ($(CRF_INC_FLAG),yes)
$(eval $(call INST_EXE_template,crf2_tool,crf2_tool,utf8))
$(eval $(call INST_EXE_template,ner_nr_tool,ner_nr_tool,utf8))
$(eval $(call INST_EXE_template,ner_tool,ner_tool,utf8))
endif

#	build_tool(crf2_tool utf8_static)
#	build_tool(ner_nr_tool utf8_static)
#	build_tool(ner_tool utf8_static)

# $(eval $(call SO_template,mod_cnqp_db,mod_cnqp_db))
# $(eval $(call EXE_template,aprtest,aprtest))

$(call INST_BIN, $(SRCTOP)/source/tools/pdc_normalize $(SRCTOP)/source/tools/pdc_tool $(SRCTOP)/source/tools/crf_tool $(SRCTOP)/source/tools/ngm_tool $(SRCTOP)/source/tools/switch_column $(SRCTOP)/source/tools/make_turing_data $(SRCTOP)/source/tools/auto_build $(SRCTOP)/source/tools/update_user_combine $(SRCTOP)/source/tools/convert_pos $(SRCTOP)/source/tools/pdc_pos_tool $(SRCTOP)/source/tools/pdc_normalize_4nr)
