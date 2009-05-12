CURDIR := $(SRCTOP)/source/lexicon/
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

$(eval $(call LIB_template,lexicon,liblexicon))
