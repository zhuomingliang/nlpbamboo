CURDIR := $(SRCTOP)/source/processor
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

PROPSO := prepare maxforward ugm_seg single_combine maxforward_combine break 

#$(foreach PROP,$(PROPSO), $(eval  \
#	$(call SO_template,$(PROP),$(PROP)_processor processor,utf8 trie)))

$(foreach PROP,$(PROPSO), $(eval  \
	$(call INST_PROCESS_SO_template,$(PROP),$(PROP)_processor processor,utf8 trie)))

CRF_SO := crf_seg crf_pos crf_ner_nr crf_ner_ns crf_ner_nt crf_ner_np crf_seg4ner  
 
ifeq ($(CRF_INC_FLAG),yes)
#$(foreach CRF,$(CRF_SO), $(eval  \
#	$(call SO_template,$(CRF),$(CRF)_processor processor,utf8 trie)))
$(foreach CRF,$(CRF_SO), $(eval  \
	$(call INST_PROCESS_SO_template,$(CRF),$(CRF)_processor processor,utf8 trie)))
endif



