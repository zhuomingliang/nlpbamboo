CURDIR := $(SRCTOP)/source/trie
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

TRIE_SRC := double_array datrie

$(eval $(call LIB_template,trie,$(TRIE_SRC)))
$(eval $(call SO_template,trie,$(TRIE_SRC)))


double_array_CFLAGS += -I$(SRCTOP)/source/mmap
datrie_CFLAGS += -I$(SRCTOP)/source/mmap


include $(SRCTOP)/source/trie/debugger/rule.mk

# $(eval $(call SO_template,mod_cnqp_db,mod_cnqp_db))
# $(eval $(call EXE_template,aprtest,aprtest))



